
#ifndef SLAVE_H
#define SLAVE_H

// Communicate with the Raspberry Pi.
class Slave {
public:
  inline Slave()  __attribute__((always_inline));
  inline void loop()  __attribute__((always_inline));
};

#endif
