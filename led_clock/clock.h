#include "FastLED.h"
#include "brightness.h"
#include "font.h"
#include "ledutils.h"
#include "ntp.h"

#ifndef __CLOCK_H__
#define __CLOCK_H__

class Clock {
public:
  Clock(uint16_t num_leds, Ntp &ntp, Ledutils &ledutils, Brightness &brightness);
  void handle(CRGB *leds);

private:
  void setPixel(CRGB *leds, uint8_t x, uint8_t y, bool on);
  void drawDigit(CRGB *leds, uint8_t digit, uint8_t x_offset);
  void drawSeparator(CRGB *leds, uint8_t x_offset);

private:
  uint16_t _num_leds;
  int8_t _prev_d = 0;
  int8_t _prev_h = 0;
  int8_t _prev_m = 0;
  unsigned long _fade = 0;
  CRGB *_leds_previous;
  CRGB *_leds_new;
  Ntp &_ntp;
  Font _font;
  Brightness &_brightness;
};

#endif //__CLOCK_H__
