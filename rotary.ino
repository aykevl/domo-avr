
#include "rotary.h"

// See:
// https://www.circuitsathome.com/mcu/programming/reading-rotary-encoder-on-arduino
// For where the code comes from, and how it works.
Rotary::Rotary() {
    // Setup encoder pins as inputs
    pinMode(ROTARY_PIN_A, INPUT_PULLUP);
    pinMode(ROTARY_PIN_B, INPUT_PULLUP);

    // Add an interrupt on both pins, TODO
    //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), rotaryInterrupt, CHANGE);
    //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), rotaryInterrupt, CHANGE);
}

void Rotary::loop() {
  // TODO: don't do it here, but in the interrupt.
  interrupt();
}

// This interrupt gets called any time the rotary encoder pins change from low
// to high, or from high to low (TODO)
void Rotary::interrupt() {
  static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t old_AB = 0;

  old_AB <<= 2;                   //remember previous state
  old_AB |= ROTARY_PIN_BITS;      //add current state
  rotaryPos += enc_states[old_AB & 0x0f];
}

// Read and reset the rotary position.
int8_t Rotary::read() {
  int8_t pos = rotaryPos;
  rotaryPos = 0;
  return pos;
}
