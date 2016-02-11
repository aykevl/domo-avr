
#ifndef ROTARY_H
#define ROTARY_H

// Hard-coded here as it is highly hardware dependent.
const uint8_t ROTARY_PIN_A = 9;
const uint8_t ROTARY_PIN_B = 10;
// This reads the bits (B000000AB) from the register.
#define ROTARY_PIN_BITS (PINB & 0b11);

class Rotary {
public:
  Rotary();
  void loop();
  void interrupt();
  int8_t read();
private:
  // this variable will be changed by encoder input in the interrupt, and must
  // thus be declared volatile. TODO: interrupts (it's not yet volatile).
  int8_t rotaryPos = 0;
};

#endif
