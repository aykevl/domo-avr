
#include "i2c.h"
#include "Wire.h"
#include <util/crc16.h>


/* The protocol
 *
 * Two types of messages: setters and getters.
 * Setters:
 *   - hue
 *   - saturation
 *   - value
 *   - time
 * Getters (first bit of command is set, thus command >= 0x80):
 *   - temperature
 *
 * The master always initiates communication.
 * With a setter:
 *   * master sends one-byte command
 *   * master sends two-byte (uin16_t, big-endian) argument
 *   * master sends one-byte crc8-ccitt of the last 3 bytes
 * With a getter:
 *   * master sends one-byte command
 *   * master sends one-byte crc8-ccitt of the command
 *   * slave sends one-byte command (in the reply)
 *   * slave sends two-byte (uint16_t, big-endian) response
 *   * slave sends one-byte crc8-ccitt of the last 3 bytes
 */


const uint8_t I2C_TYPE               = 0b11000000;
const uint8_t I2C_TYPE_GETTER_FLAG   = 0b00000000;
const uint8_t I2C_TYPE_GETTER2       = 0b00000000;
const uint8_t I2C_TYPE_GETTER4       = 0b01000000;
const uint8_t I2C_TYPE_SETTER_FLAG   = 0b10000000;
const uint8_t I2C_TYPE_SETTER2       = 0b10000000;
const uint8_t I2C_TYPE_SETTER4       = 0b11000000;

const uint8_t I2C_COMMAND_COLOR      = 0x05; // arg: raw color value (type, hue/red, saturation/green, value/blue)
const uint8_t I2C_COMMAND_TEMP_NOW   = 0x11; // response: temperature in tenth degrees Celsius
const uint8_t I2C_COMMAND_TEMP_AVG   = 0x12; // response: same, but averaged over 2.5s
const uint8_t I2C_COMMAND_TEMP_RAW   = 0x13; // response: raw analogRead() value
const uint8_t I2C_COMMAND_TEST       = 0x20; // response: 0xabcd or 0x1234abcd


I2C::I2C(uint8_t address) {
  tempSetup();
  Wire.begin(address);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
}

void onReceive(int count) {
  // Do nothing (it interrupts onRequest)
}

void onRequest() {
  if (Wire.available() < 1) {
    return;
  }

  uint8_t command = Wire.read();

  uint8_t commandType = command & I2C_TYPE; // only take what is in the type
  uint8_t commandWithoutType = command & ~I2C_TYPE; // remove type bit

  if (commandType == I2C_TYPE_GETTER2 || commandType == I2C_TYPE_GETTER4) {
    // getter

    uint32_t response = 0;
    bool hasResponse = true;

    switch (commandWithoutType) {
      case I2C_COMMAND_TEMP_NOW:
        // Send as centidegrees, where 0 degrees is -55°C (218.15K)
        response = (tempRead() - 218.15) * 100.0 + 0.5;
        break;
      case I2C_COMMAND_TEMP_AVG:
        // Send as centidegrees, where 0 degrees is -55°C (218.15K)
        response = (tempReadAvg() - 218.15) * 100.0 + 0.5;
        break;
      case I2C_COMMAND_TEMP_RAW:
        // Send as centidegrees, where 0 degrees is -55°C (218.15K)
        response = tempReadRaw();
        break;
      case I2C_COMMAND_COLOR:
        response = leds.getRawColor();
        break;
      case I2C_COMMAND_TEST:
        response = 0x1234abcd;
        break;
      default:
        response = 0xdead0000 | command;
        hasResponse = false;
        break;
    }

    uint8_t response_crc = _crc8_ccitt_update(response_crc, command);

    uint8_t len = 0;
    if (commandType == I2C_TYPE_GETTER2) {
      len = 2;
    } else if (commandType == I2C_TYPE_GETTER4) {
      len = 4;
    }
    uint8_t buf[len+1]; // add CRC
    for (uint8_t i=0; i<len; i++) {
      buf[i] = response;
      response_crc = _crc8_ccitt_update(response_crc, response);
      response >>= 8;
    }
    buf[len] = response_crc;

    Wire.write(buf, len+1);

  } else if (commandType == I2C_TYPE_SETTER4) {
    // 32-bit setter

    uint8_t crc2 = _crc8_ccitt_update(0, command); // first byte of locally calculated CRC

    uint32_t arg = 0;
    for (uint8_t i=0; i<4; i++) {
      arg >>= 8;
      uint32_t c = Wire.read();
      crc2 = _crc8_ccitt_update(crc2, c);
      arg |= c << 24;
    }

    uint8_t crc = Wire.read();
    if (crc != crc2) {
      // try again next time
      return;
    }

    switch(commandWithoutType) {
      case I2C_COMMAND_COLOR:
        leds.setRawColor(arg);
        break;
    }
  }
}
