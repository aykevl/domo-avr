
#ifndef COLOR_H
#define COLOR_H

#include <Arduino.h>
#include "fastled_hsv2rgb.h"
#include "ufloat8.h"

typedef struct {
  union {
    uint8_t red;
    uint8_t r;
  };
  union {
    uint8_t green;
    uint8_t g;
  };
  union {
    uint8_t blue;
    uint8_t b;
  };
} colorRGB;

// hue:        0-255
// saturation: 0-255
// luminance:  0-255
typedef struct {
  union {
    uint8_t hue;
    uint8_t h;
  };
  union {
    uint8_t saturation;
    uint8_t s;
  };
  union {
    uint8_t value;
    uint8_t v;
  };
} colorHSV;

/* Type parameter (where 0 is the least significant bit):
 * bit 0-1: 2-bit color mode (RGB, HSV, HSV_MAX, undefined)
 * bit 6:   hue is time flag (saved with compression)
 * bit 7:   white flag (for testing)
 */

const uint8_t COLOR_FLAG_WHITE     = 0b10000000;
const uint8_t COLOR_FLAG_HUETIME   = 0b01000000;
const uint8_t COLOR_MODE_MASK      = 0b00000011; // bit 7-8: mode
const uint8_t COLOR_MODE_RGB       = 0b00000000; // mode value 0
const uint8_t COLOR_MODE_HSV       = 0b00000001; // mode value 1
const uint8_t COLOR_MODE_HSV_MAX   = 0b00000010; // mode value 2
const uint8_t COLOR_MODE_UNDEFINED = 0b00000011; // mode value 3 (undefined)

typedef struct Color {
  union {
    struct {
      uint8_t type;
      union {
        uint8_t compressedTime;
        uint8_t hue;
        uint8_t red;
      };
      union {
        uint8_t saturation;
        uint8_t green;
      };
      union {
        uint8_t value;
        uint8_t blue;
      };
    };
    uint32_t raw;
  };
  uint8_t loopHue = 0;

  inline Color() {
    // TODO put back some zero initialization here
  }

  inline Color(uint8_t _type, uint8_t _hue, uint8_t _saturation, uint8_t _value) {
    type = _type;
    hue = _hue;
    saturation = _saturation;
    value = _value;
  }

  inline bool isWhite() {
    return type & COLOR_FLAG_WHITE;
  }

  inline bool hueIsTime() {
    return type & COLOR_FLAG_HUETIME;
  }

  inline bool isHSV() {
    uint8_t m = getMode();
    return m == COLOR_MODE_HSV || m == COLOR_MODE_HSV_MAX;
  }

  // Return the time in milliseconds
  inline uint32_t getTime() {
    if (!hueIsTime()) {
      return 0;
    }
    return ufloat8_dec(compressedTime) << 8;
  }

  // Set the time in milliseconds
  inline void setTime(uint32_t time) {
    if (!hueIsTime()) {
      // set time flag
      type = type | COLOR_FLAG_HUETIME;
    }
    this->compressedTime = ufloat8_enc(time >> 8);
  }

  inline void incTime(int8_t inc) {
    if (!hueIsTime()) {
      this->loopHue = this->hue;
      // set time flag
      type = type | COLOR_FLAG_HUETIME;
      compressedTime = 0x28; // 32, or ~8s
      return;
    }
    uint8_t compressedTime2 = compressedTime + inc;
    if (inc < 0 && compressedTime2 > compressedTime) {
      // wrap-around detected
      compressedTime2 = 0;
    }
    compressedTime = compressedTime2;
  }

  inline void incHue(int8_t inc) {
    if (hueIsTime()) {
      // remove time flag
      type = type & ~COLOR_FLAG_HUETIME;
      this->hue = this->loopHue;
    }
    this->hue += inc;
  }

  inline uint8_t getMode() {
    return type & COLOR_MODE_MASK;
  }

  inline uint8_t hueIncrement() {
    this->loopHue++;
  }

  // Use FastLED to calculate color values
  // @hue: the hue to use if hue is the compressed time
  inline colorRGB getRGB() {
    uint8_t mode = getMode();
    if (isWhite()) {
      return colorRGB{0xff, 0xff, 0xff};
    }
    if (mode == COLOR_MODE_RGB) {
      return colorRGB{red, green, blue};
    }
    uint8_t hue = this->loopHue;
    if (!hueIsTime()) {
      hue = this->hue;
    }
    const CHSV fl_hsv = CHSV{
      hue,
      saturation,
      value,
    };
    CRGB fl_rgb;
    hsv2rgb_rainbow(fl_hsv, fl_rgb);
    if (mode == COLOR_MODE_HSV_MAX && value == 0xff) {
      fl_rgb.maximizeBrightness();
    }
    return colorRGB{
      fl_rgb.red,
      fl_rgb.green,
      fl_rgb.blue,
    };
  };
} Color;

#endif
