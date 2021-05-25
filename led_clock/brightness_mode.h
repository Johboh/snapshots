#include "FastLED.h"
#include "brightness.h"
#include "font.h"
#include "ledutils.h"
#include <Arduino.h>

#ifndef __BRIGHTNESS_MODE_H__
#define __BRIGHTNESS_MODE_H__

class BrightnessMode {
public:
  BrightnessMode(uint16_t num_leds, Ledutils &ledutils, Brightness &brightness);
  void handle(CRGB *leds);
  void increaseBrightness();
  void decreaseBrightness();
  void reset();

private:
  void detectManualBrightness();

  uint16_t _num_leds;
  Font _font;
  Brightness &_brightness;
  unsigned long _last_switch_ms = 0;
  bool _use_adc = true;
  bool _manual_brightness = false;
  uint8_t _current_brightness = 128;
};

#endif //__BRIGHTNESS_MODE_H__
