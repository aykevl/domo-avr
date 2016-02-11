
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
  if (previousState == currentState) {
    return 0;
  }
  previousState = currentState;
  return currentState;
}
