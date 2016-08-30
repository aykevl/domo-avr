/* spi.h
 *
 * Source:
 * https://github.com/aykevl/arduino-WireSPI
 *
 * Copyright 2016 Ayke van Laethem. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE.txt file.
 */

#ifndef SPI_H
#define SPI_H

const uint8_t SPI_BUFFER_SIZE = 32;

class WireSPI {
public:
  WireSPI();
  uint8_t available();
  uint8_t read();
  void write(uint8_t);
  void write(const uint8_t *, uint8_t);
  inline void __interrupt(); // do not call this function

private:
  volatile bool sending; // is the SPDR register holding data?
  char sendBuf[SPI_BUFFER_SIZE];
  volatile uint8_t sendBeginPos;
  volatile uint8_t sendEndPos;
  char recvBuf[SPI_BUFFER_SIZE];
  volatile uint8_t recvBeginPos;
  volatile uint8_t recvEndPos;

  uint8_t availableToSend();
};

// Use this object, do not instantiate the class yourself.
extern WireSPI SPI;

#endif
