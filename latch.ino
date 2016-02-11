
#include "latch.h"

Latch::Latch(uint8_t pin) {
  button = Button(pin);
}

void Latch::loop() {
  button.loop();
}

// Returns:
//  0: no change
//  1: short press
//  2: long press
uint8_t Latch::read() {
  uint8_t currentState = button.read();
  uint8_t ret = 0;
  if (currentState == 2) {
    ret = 2;
  } else if (previousState == 1 && currentState == 0) {
    ret = 1;
  }
  previousState = currentState;
  return ret;
}
