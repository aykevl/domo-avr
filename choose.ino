
#include "choose.h"

Choose::Choose() {
}

Choose::Choose(uint8_t pin, uint8_t states) {
  this->button = Button(pin);
  this->states = states;
}

void Choose::loop() {
  button.loop();
  switch (button.read()) {
    case 0: // released
      pressed = false;
      break;
    case 1: // pressed
      if (!pressed) {
        // next cycle
        pressed = true;
        state++;
        if (state >= states) {
          state = 0;
        }
      }
      break;
    case 2: // long pressed
      break;
  }
}

// Returns the state number when the state changed, or 0 if it didn't change,
// and -1 if the button was long pressed.
uint8_t Choose::read() {
  return state;
}
