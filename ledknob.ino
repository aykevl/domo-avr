#include "ledknob.h"

LedKnob::LedKnob(uint8_t buttonPin, uint8_t ledLoopPin) {
  choose = Choose(buttonPin, LEDKNOB_STATES);
  ledLoop = Loop(ledLoopPin, LEDKNOB_STATES);
}

void LedKnob::loop() {
  rotary.loop();
  choose.loop();

  uint8_t state = choose.read();

  // Set the led button to this state
  while (state != ledLoop.getPosition()) {
    ledLoop.increment();
  }

  int8_t rotaryValue = rotary.read();
  if (!rotaryValue) return;

  if (state == LEDKNOB_HUE) {
    leds.incHue(rotaryValue);
  } else if (state == LEDKNOB_TIME) {
    // value=1 means 256ms, value=2 512ms, value3=768ms etc.
    leds.incTime(rotaryValue);
  } else {
    // Read the current value.
    uint8_t oldValue;
    switch (state) {
      case LEDKNOB_SATURATION:
        oldValue = leds.getSaturation();
        break;
      case LEDKNOB_VALUE:
        oldValue = leds.getValue();
        break;
      default:
        return;
    }

    // Apply rotation.
    uint8_t newValue = oldValue + rotaryValue * 2;

    // Clamp in the uint8_t range. Hue must of course not be clamped, 0xff and
    // 0x00 are very close to each other on the color wheel.
    if (rotaryValue > 0 && newValue < oldValue) {
      newValue = 0xff;
    } else if (rotaryValue < 0 && newValue > oldValue) {
      newValue = 0;
    }

    // Set the state.
    switch (state) {
      case LEDKNOB_SATURATION:
        leds.setSaturation(newValue);
        break;
      case LEDKNOB_VALUE:
        leds.setValue(newValue);
        break;
      default:
        break;
    }
  }
}
