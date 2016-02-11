
#include "i2c.h"
#include "TinyWireS.h"
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

const uint8_t I2C_COMMAND_HUE        = 0x01; // arg: 0-255 on the color wheel
const uint8_t I2C_COMMAND_SATURATION = 0x02; // arg: 0-255 colorless to color
const uint8_t I2C_COMMAND_VALUE      = 0x03; // arg: 0-255 dark to light
//const uint8_t I2C_COMMAND_TIME       = 0x04; // arg: number of seconds
const uint8_t I2C_COMMAND_COLOR      = 0x05; // arg: raw color value (type, hue/red, saturation/green, value/blue)
const uint8_t I2C_COMMAND_COLOR_HI   = 0x06; // arg: high biths of I2C_COMMAND_COLOR
const uint8_t I2C_COMMAND_TEMP_NOW   = 0x11; // response: temperature in tenth degrees Celsius
const uint8_t I2C_COMMAND_TEMP_AVG   = 0x12; // response: same, but averaged over 2.5s
const uint8_t I2C_COMMAND_TEMP_RAW   = 0x13; // response: raw analogRead() value
const uint8_t I2C_COMMAND_TEST       = 0x20; // response: 0xabcd or 0x1234abcd

const unsigned int I2C_TIMEOUT = 200; // milliseconds


I2C::I2C(uint8_t address) {
  TinyWireS.begin(address);
  tempSetup();
}

void I2C::loop() {
  /* The loop.
   *
   * `command` is non-zero when a command is received an we're waiting on the contents.
   * Otherwise, we're waiting for a new command.
   * So, when the command body is received, we'll set the command back to zero.
   */
  static uint8_t command = 0;
  static uint8_t crc2; // locally calculated CRC
  static unsigned long lastMillis = 0;

  tempLoop();

  if (!command && TinyWireS.available() > 0) {
    command = TinyWireS.receive();
    crc2 = _crc8_ccitt_update(0, command); // first byte of locally calculated CRC
    lastMillis = millis();
  }

  uint8_t commandType = command & I2C_TYPE; // only take what is in the type
  uint8_t commandWithoutType = command & ~I2C_TYPE; // remove type bit

  // if this is a getter
  if ((commandType == I2C_TYPE_GETTER2 || commandType == I2C_TYPE_GETTER4) && TinyWireS.available() >= 1) {
    // check CRC of command
    uint8_t crc = TinyWireS.receive();
    if (crc != crc2) {
      // don't send anything back
      command = 0;
      return;
    }

    bool hasResponse = true;

    uint32_t response = 0;
    switch (commandWithoutType) {
      /*case I2C_COMMAND_HUE:
        response = leds.getHue();
        break;*/
      /*case I2C_COMMAND_SATURATION:
        response = leds.getSaturation();
        break;
      case I2C_COMMAND_VALUE:
        response = leds.getValue();
        break;*/
      /*case I2C_COMMAND_TIME:
        // Set the time in seconds, max. time is thus about 18 hours (65536s).
        response = leds.getTime() / 128;
        break;*/
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
      /*case I2C_COMMAND_COLOR_HI:
        response = leds.getRawColor();
        response >>= 16;
        break;*/
      case I2C_COMMAND_TEST:
        response = 0x1234abcd;
        break;
      default:
        hasResponse = false;
        break;
    }

    // only send a response with recognized commands
    if (!hasResponse) {
      response = 0xdead0000 | command;
    }

    // Send the response as an unsigned 32-bit little-endian integer
    // (least significant digits first).
    uint8_t response_crc = 0;
    response_crc = _crc8_ccitt_update(response_crc, command);
    uint8_t i = 2; // send 2 bytes
    if (commandType == I2C_TYPE_GETTER4) {
      i = 0; // send 4 bytes
    }
    for (; i<4; i++) {
      TinyWireS.send(response); // least significant bits first
      response_crc = _crc8_ccitt_update(response_crc, response);
      response >>= 8;
    }
    TinyWireS.send(response_crc);
    command = 0;
    return;
  }

  // if this is a 16-bit setter
  if (commandType == I2C_TYPE_SETTER2 && TinyWireS.available() >= 3) {
    uint16_t arg = 0;
    for (uint8_t i=0; i<2; i++) {
      arg >>= 8;
      uint16_t c = TinyWireS.receive();
      crc2 = _crc8_ccitt_update(crc2, c);
      arg |= c << 8; // add to higher bit
    }

    uint8_t crc = TinyWireS.receive();
    if (crc != crc2) {
      // try again next time
      command = 0;
      return;
    }

    switch (commandWithoutType) {
      /*case I2C_COMMAND_HUE:
        leds.setHue(arg);
        break;*/
      /*case I2C_COMMAND_SATURATION:
        leds.setSaturation(arg);
        break;
      case I2C_COMMAND_VALUE:
        leds.setValue(arg);
        break;*/
      case I2C_COMMAND_COLOR:
        leds.setRawColor(arg | (leds.getRawColor() & 0xffff0000));
        break;
      /*case I2C_COMMAND_COLOR_HI:
        leds.setRawColor((uint32_t)arg << 16  | leds.getRawColor() & 0x0000ffff);
        break;*/
      /*case I2C_COMMAND_TIME:
        // Set the time in steps of 128ms
        leds.setTime((unsigned long)(arg));
        break;*/
    }

    command = 0;

    return;
  }

  // 32-bit setter
  if (commandType == I2C_TYPE_SETTER4 && TinyWireS.available() >= 5) {
    uint32_t arg = 0;
    for (uint8_t i=0; i<4; i++) {
      arg >>= 8;
      uint32_t c = TinyWireS.receive();
      crc2 = _crc8_ccitt_update(crc2, c);
      arg |= c << 24;
    }

    uint8_t crc = TinyWireS.receive();
    if (crc != crc2) {
      // try again next time
      command = 0;
      return;
    }

    switch(commandWithoutType) {
      case I2C_COMMAND_COLOR:
        leds.setRawColor(arg);
        break;
    }

    command = 0;

    return;
  }

  if (command && millis() >= lastMillis + I2C_TIMEOUT) {
    // timeout reached before getting full command, reset
    command = 0;
    while (TinyWireS.available()) {
      TinyWireS.receive();
    }
  }
}

