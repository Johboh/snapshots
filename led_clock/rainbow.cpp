#include "rainbow.h"

Rainbow::Rainbow(uint16_t num_leds) : _num_leds(num_leds) {}

// Mostly copy from the Arduino FastLed rainbow examples.
void Rainbow::handle(CRGB *leds) {
  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = _hue16; // gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - _last_millis;
  _last_millis = ms;
  _pseudotime += deltams * msmultiplier;
  _hue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = _pseudotime;

  for (uint16_t i = 0; i < _num_leds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (_num_leds - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}

void Rainbow::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  FastLED.setBrightness(brightness);
}
uint8_t Rainbow::getBrightness() { return _brightness; }