/* spi.cpp
 *
 * Source:
 * https://github.com/aykevl/arduino-WireSPI
 *
 * Copyright 2016 Ayke van Laethem. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE.txt file.
 */

#include <Arduino.h>
#include "spi.h"

// See: http://www.gammon.com.au/spi
WireSPI::WireSPI() {
  // initialize variables (empty buffers)
  sending = false;
  sendBeginPos = 0;
  sendEndPos = 0;
  recvBeginPos = 0;
  recvEndPos = 0;

  // Set the slave pin as output
  pinMode(MISO, OUTPUT);

  // Turn on SPI in slave mode
  SPCR |= bit (SPE);
  SPCR |= bit (SPIE);
}

ISR(SPI_STC_vect) {
  // This is the actual interrupt.
  SPI.__interrupt();
}

// This is the interrupt from within the class.
inline void WireSPI::__interrupt() {
  if (sending) {
    // We're sending bytes.

    if (availableToSend() > 0) {
      // set next byte in the register to send
      SPDR = sendBuf[sendBeginPos];
      sendBeginPos = (sendBeginPos + 1) % SPI_BUFFER_SIZE;
    } else {
      // Send 0 when there is no data to send
      SPDR = 0;
      sending = false;
    }

  } else {
    // We're receiving a byte.

    // Add to receive buffer if there's room.
    if (available() < SPI_BUFFER_SIZE) {
      // Grab received byte from SPI Data Register.
      recvBuf[recvEndPos] = SPDR;
      recvEndPos = (recvEndPos + 1) % SPI_BUFFER_SIZE;
    }

    // Send 0 when there is no data to send.
    SPDR = 0;
  }
}

uint8_t WireSPI::available() {
  return (recvEndPos - recvBeginPos) % SPI_BUFFER_SIZE;
}

uint8_t WireSPI::availableToSend() {
  return (sendEndPos - sendBeginPos) % SPI_BUFFER_SIZE;
}

uint8_t WireSPI::read() {
  while (available() < 1) {
    // wait until a byte is available
  }
  uint8_t c = recvBuf[recvBeginPos];
  recvBeginPos = (recvBeginPos + 1) % SPI_BUFFER_SIZE;
  return c;
}

void WireSPI::write(uint8_t c) {
  while (availableToSend()+1 >= SPI_BUFFER_SIZE) {
    // Wait until the buffer has cleared.
    // TODO: I think this overflow detection doesn't actually work.
  }
  sendBuf[sendEndPos] = c;
  sendEndPos = (sendEndPos + 1) % SPI_BUFFER_SIZE;

  // Set next byte in the register to send if we're not already sending.
  if (!sending) {
    SPDR = sendBuf[sendBeginPos];
    sendBeginPos = (sendBeginPos + 1) % SPI_BUFFER_SIZE;
    sending = true;
  }
}

void WireSPI::write(const uint8_t *buf, uint8_t len) {
  for (uint8_t i=0; i<len; i++) {
    write(buf[i]);
  }
}

// Preinstantiate object
WireSPI SPI = WireSPI();
