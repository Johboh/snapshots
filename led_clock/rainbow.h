#include "FastLED.h"
#include <Arduino.h>

#ifndef __RAINBOW_H__
#define __RAINBOW_H__

class Rainbow {
public:
  Rainbow(uint16_t num_leds);
  void handle(CRGB *leds);
  void setBrightness(uint8_t brightness);
  uint8_t getBrightness();

private:
  uint16_t _num_leds;
  uint16_t _pseudotime = 0;
  uint16_t _last_millis = 0;
  uint16_t _hue16 = 0;
  uint8_t _brightness = 200;
};

#endif //__RAINBOW_H__
