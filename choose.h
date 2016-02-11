
#ifndef SWITCH_H
#define SWITCH_H

#include "button.h"

class Choose {
public:
  inline Choose(); // dummy
  inline Choose(uint8_t, uint8_t);
  inline void loop();
  inline uint8_t read();
private:
  uint8_t state = 0;
  uint8_t states;
  bool pressed = false;
  Button button;
};

#endif
