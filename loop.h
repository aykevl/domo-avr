
#ifndef LOOP_H
#define LOOP_H

class Loop {
public:
  inline Loop(uint8_t, uint8_t);
  inline Loop();
  void increment();
  uint8_t getPosition();
  uint8_t getLength();
private:
  uint8_t pin;
  uint8_t len;
  uint8_t pos = 0;
};

#endif LOOP_H
