#include "FastLED.h"
#include "ledutils.h"
#include <Arduino.h>

#ifndef __FONT_H__
#define __FONT_H__

// clang-format off
const uint8_t digits[10][5][3] = {
  {
    {1, 1, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  {
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  {
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1}
  },
  {
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
  },
  {
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
  },
  {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  {
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
  }
};

const uint8_t separator[5] = {
  0,
  1,
  0,
  1,
  0
};
// clang-format on

class Font {
public:
  Font(CRGBPalette16 palette, Ledutils &ledutils);
  void drawDigit(CRGB *leds, uint8_t digit, uint8_t x_offset, uint16_t pallette_offset = 0);
  void drawSeparator(CRGB *leds, uint8_t x_offset, uint16_t pallette_offset = 0);
  void setBrightness(uint8_t brightness);
  void useFullWhiteInsteadOfPalette(bool use_white);

private:
  void setPixel(CRGB *leds, uint8_t x, uint8_t y, bool on, uint16_t pallette_offset);

private:
  CRGBPalette16 _palette;
  Ledutils &_ledutils;
  uint8_t _brightness = 0;
  uint16_t _time_offset = 0;
  bool _white_instead_of_palette = false;
};

#endif //__FONT_H__
