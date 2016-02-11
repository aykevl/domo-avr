
#ifndef SWITCH2_H
#define SWITCH2_H

#include "button.h"

class Latch {
public:
  inline Latch(uint8_t);
  void loop();
  uint8_t read();
private:
  Button button;
  uint8_t previousState = 0;
};

#endif
