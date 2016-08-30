# Domotica on AVR

This is the AVR (Atmel/Arduino) part of a small home automation system.

Libraries used:

  * [FastLED](https://github.com/FastLED/FastLED) for fast LED color
    calculation. I only used part of the system, to turn HSV into RGB.
  * [ufloat8](https://github.com/aykevl/ufloat8) to compress a very wide range
    (± 20 bits) into a small 8-bit nuber. This is how I compress cycle duration
    of the LED color loop.
  * [WireSPI](https://github.com/aykevl/arduino-WireSPI) to communicate with a
    Raspberry Pi as a slave.

A Raspberry Pi gathers temperature information and issues commands on the
microcontroller with [domo-rpi](https://github.com/aykevl/domo-rpi).
