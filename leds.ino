
#include "leds.h"

// PWM pins (red, green, blue)
const uint8_t LED_PINS[3] = {5, 7, 8};


Leds::Leds(uint8_t mode, uint8_t hue, uint8_t saturation, uint8_t value) {
  this->color.type = mode;
  this->color.hue = hue;
  this->color.saturation = saturation;
  this->color.value = value;
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  update();
}

unsigned long Leds::getTime() {
  return color.getTime();
}

void Leds::setTime(unsigned long time) {
  color.setTime(time);
}

void Leds::incTime(int8_t inc) {
  color.incTime(inc);
}

void Leds::incHue(int8_t inc) {
  color.incHue(inc);
  update();
}

uint8_t Leds::getSaturation() {
  return color.saturation;
}

void Leds::setSaturation(uint8_t saturation) {
  color.saturation = saturation;
  update();
}

uint8_t Leds::getValue() {
  return color.value;
}

void Leds::setValue(uint8_t value) {
  color.value = value;
  update();
}

uint32_t Leds::getRawColor() {
  // Unfortunately, there's no way to determine the endianness :(
  //return color.raw;
  return (uint32_t(color.type) << 24) |
         (uint32_t(color.hue) << 16) |
         (uint32_t(color.saturation) << 8) |
         (uint32_t(color.value));
}

void Leds::setRawColor(uint32_t raw) {
  //color.raw = raw;
  color.type       = raw >> 24;
  color.hue        = raw >> 16;
  color.saturation = raw >> 8;
  color.value      = raw;
  update();
}

bool Leds::hueIsTime() {
  return color.hueIsTime();
}

bool Leds::isWhite() {
  return color.isWhite();
}

void Leds::setWhite(bool white) {
  if (color.isWhite() != white) {
    // Toggle white state
    color.type ^= COLOR_FLAG_WHITE;
    update();
  }
}

void Leds::setLedsRGB(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(LED_PINS[0], red);
  analogWrite(LED_PINS[1], green);
  analogWrite(LED_PINS[2], blue);
}

void Leds::update() {
  const colorRGB rgb = color.getRGB();
  setLedsRGB(rgb.red, rgb.green, rgb.blue);
}

void Leds::loop() {
  // Do this in a quick operation:
  //  time_ms = color.getTime();
  //  time_us = time_ms * 1000;  // approximated via *1024 or <<10
  //  interval = time_us / 256;  // time per color shade (hue)
  unsigned long loopTimeInterval = color.getTime() << 2;

  if (color.isHSV() && color.value == 0) return;

  unsigned long currentMicros = micros();
  // Doing (current - last >= interval) takes care of timer rollovers.
  if (currentMicros - lastMicros >= loopTimeInterval) {
    lastMicros = currentMicros;
    color.hueIncrement();
    update();
  }
}

