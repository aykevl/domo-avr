
#include "loop.h"

void Loop::setup(uint8_t pinClock, uint8_t pinReset, uint8_t len) {
  this->pinClock = pinClock;
  this->pinReset = pinReset;
  this->len = len;

  pinMode(pinClock, OUTPUT);
  pinMode(pinReset, OUTPUT);

  digitalWrite(pinClock, LOW);

  // Reset the device on startup
  digitalWrite(pinReset, HIGH);
  digitalWrite(pinReset, LOW);
}

void Loop::increment() {
  // Increase counter.
  this->pos++;
  if (this->pos >= this->len) {
    // wrap around
    this->pos = 0;
  }

  // Generate a single clock pulse.
  digitalWrite(pinClock, HIGH);
  digitalWrite(pinClock, LOW);

  if (this->pos == 0) {
    // Reset the LEDs to the first (all off).
    digitalWrite(pinReset, HIGH);
    digitalWrite(pinReset, LOW);
  }
}

uint8_t Loop::getPosition() {
  return pos;
}
