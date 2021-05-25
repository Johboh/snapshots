#include "singlecolor.h"

Singlecolor::Singlecolor(uint16_t num_leds) : _num_leds(num_leds) {}

void Singlecolor::handle(CRGB *leds) {
  for (uint16_t i = 0; i < _num_leds; i++) {
    leds[i] = CRGB::White;
  }
}
