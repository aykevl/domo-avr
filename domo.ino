
#include "color.h"
#include "ledknob.h"
#include "latch.h"
#include "leds.h"
#include "slave.h"

Leds leds = Leds(COLOR_MODE_HSV | COLOR_FLAG_HUETIME,
                 0x48,  // hue/time (0x48 = 128 = ~32s)
                 0xff,  // saturation
                 0xff); // value

int main() {
  init();
  Slave slave;
  LedKnob ledknob(3, 0);
  Latch latch(2);

  for (;;) {
    tempLoop();
    leds.loop();
    ledknob.loop();
    latch.loop();
    slave.loop();

    // Button press handler
    switch (latch.read()) {
      case 0: // no change
        break;
      case 1: // short press
        if (leds.isWhite()) {
          leds.setWhite(false);
        } else {
          if (leds.getValue()) {
            leds.setValue(0x00);
          } else {
            leds.setValue(0xff);
          }
        }
        break;
      case 2: // long press
        leds.setWhite(true);
        break;
    }
  }
}
