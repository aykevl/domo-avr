
#ifndef LEDS_H
#define LEDS_H

#include "color.h"

class Leds {
public:
  inline Leds(uint8_t, uint8_t, uint8_t, uint8_t);
  inline void loop();
  inline unsigned long getTime();
  inline void setTime(unsigned long);
  inline void incTime(int8_t);
  inline void incHue(int8_t);
  inline uint8_t getSaturation();
  inline void setSaturation(uint8_t);
  inline uint8_t getValue();
  inline void setValue(uint8_t);
  inline uint32_t getRawColor();
  inline void setRawColor(uint32_t);
  inline bool hueIsTime();
  inline void setWhite(bool);
  inline void setLedsRGB(uint8_t red, uint8_t green, uint8_t blue);
private:
  unsigned long lastMicros = 0;
  void update();
  Color color;
};

#endif
