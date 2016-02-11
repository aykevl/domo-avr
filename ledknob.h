
#ifndef LEDKNOB_H
#define LEDKNOB_H

#include "rotary.h"
#include "choose.h"
#include "loop.h"

const uint8_t LEDKNOB_HUE        = 1;
const uint8_t LEDKNOB_SATURATION = 2;
const uint8_t LEDKNOB_VALUE      = 3;
const uint8_t LEDKNOB_TIME       = 4;
const uint8_t LEDKNOB_STATES     = 5; // amount of states

class LedKnob {
public:
  LedKnob(uint8_t, uint8_t);
  void loop();
private:
  Rotary rotary;
  Choose choose;
  Loop ledLoop;
};

#endif
