
#include "color.h"


// Taken from:
// http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
// TODO: convert this to an integer calculation.
// http://www.had2know.com/technology/hsi-rgb-color-converter-equations.html
/*colorRGB colorHSVtoRGB(colorHSV c) {
  float H = c.hue * PI / COLOR_MAXHUE * 2.0;
  float S = c.saturation / 255.0;
  float I = c.intensity / 255.0;
  uint8_t r, g, b;

  const uint16_t huePart = COLOR_MAXHUE / 3;
  const float    hueToRad = PI / COLOR_MAXHUE * 2.0;

  // Math! Thanks in part to Kyle Miller.
  if (c.hue < huePart) {
    float x = cos(c.hue * hueToRad) / cos((huePart/2 - c.hue) * hueToRad);
    r = 255 * I / 3 * (1 + S * x);
    g = 255 * I / 3 * (1 + S * (1 - x));
    b = 255 * I / 3 * (1 - S);
  } else if (c.hue < huePart*2) {
    c.hue -= huePart;
    float x = cos(c.hue * hueToRad) / cos((huePart/2 - c.hue) * hueToRad);
    g = 255 * I / 3 * (1 + S * x);
    b = 255 * I / 3 * (1 + S * (1 - x));
    r = 255 * I / 3 * (1 - S);
  } else {
    c.hue -= huePart*2;
    float x = cos(c.hue * hueToRad) / cos((huePart/2 - c.hue) * hueToRad);
    b = 255 * I / 3 * (1 + S * x);
    r = 255 * I / 3 * (1 + S * (1 - x));
    g = 255 * I / 3 * (1 - S);
  }
  return colorRGB {
    r,
    g,
    b,
  };
}*/
