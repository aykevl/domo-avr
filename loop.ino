
#include "loop.h"

Loop::Loop() {
  // dummy
}

Loop::Loop(uint8_t pin, uint8_t len) {
  this->pin = pin;
  this->len = len;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Loop::increment() {
  this->pos++;
  if (this->pos >= this->len) {
    this->pos = 0;
  }
  if (this->pos == 0) {
    // Disable power, resetting the 4066 IC.
    digitalWrite(pin, LOW);
  } else if (this->pos == 1) {
    digitalWrite(pin, HIGH);
  } else {
    // The 4066 IC responds really fast (or digitalWrite is just slow).
    digitalWrite(pin, LOW);
    digitalWrite(pin, HIGH);
  }
}

uint8_t Loop::getPosition() {
  return pos;
}

uint8_t Loop::getLength() {
  return len;
}
