#include "FastLED.h"
#include <Arduino.h>

#ifndef __SINGLECOLOR_H__
#define __SINGLECOLOR_H__

class Singlecolor {
public:
  Singlecolor(uint16_t num_leds);
  void handle(CRGB *leds);

private:
  uint16_t _num_leds;
};

#endif //__SINGLECOLOR_H__
