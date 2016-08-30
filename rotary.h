
#ifndef ROTARY_H
#define ROTARY_H

#if defined(__AVR_ATmega328P__)
const uint8_t ROTARY_PIN_A = 0;
const uint8_t ROTARY_PIN_B = 1;
// This reads bits 0bBA000000 from the register.
#define ROTARY_PIN_BITS (PIND & 0b00000011)

#elif defined(__AVR_ATtiny84__)
const uint8_t ROTARY_PIN_A = 9;
const uint8_t ROTARY_PIN_B = 10;
// This reads bits 0b000000AB from the register.
#define ROTARY_PIN_BITS (PINB & 0b00000011)

#endif


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
