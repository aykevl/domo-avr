
#ifndef I2C_H
#define I2C_H

struct I2C {
public:
  inline I2C(uint8_t)  __attribute__((always_inline));
  inline void loop()  __attribute__((always_inline));
};

#endif
