
#ifndef LOOP_H
#define LOOP_H

class Loop {
public:
  inline void setup(uint8_t, uint8_t, uint8_t);
  void increment();
  uint8_t getPosition();
private:
  uint8_t pinClock;
  uint8_t pinReset;
  uint8_t len;
  uint8_t pos = 0;
};

#endif LOOP_H
