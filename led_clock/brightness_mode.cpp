#include "brightness_mode.h"

#define SWITCH_EVERY_MS 3000

BrightnessMode::BrightnessMode(uint16_t num_leds, Ledutils &ledutils, Brightness &brightness)
    : _num_leds(num_leds), _font(Font(RainbowColors_p, ledutils)), _brightness(brightness) {}

void BrightnessMode::handle(CRGB *leds) {
  if (!_manual_brightness) {
    _current_brightness = _brightness.getBrightness();
  }
  _font.setBrightness(_current_brightness);
  for (uint16_t i = 0; i < _num_leds; ++i) {
    leds[i] = CRGB::Black;
  }

  unsigned int val = _use_adc ? _brightness.getLatestAdcValue() : _current_brightness;
  leds[0] = _use_adc ? CRGB::Black : CRGB::Red;
  leds[1] = _use_adc ? CRGB::Black : CRGB::Green;
  leds[2] = _use_adc ? CRGB::Black : CRGB::Blue;
  leds[3] = _use_adc ? CRGB::Black : CRGB::Yellow;

  char buf[8];
  sprintf(buf, "%04d", val);
  _font.drawDigit(leds, buf[0] - 48, 2, 128);
  _font.drawDigit(leds, buf[1] - 48, 6, 128);
  _font.drawDigit(leds, buf[2] - 48, 10, 128);
  _font.drawDigit(leds, buf[3] - 48, 14, 128);
  delay(500);

  if (millis() - _last_switch_ms > SWITCH_EVERY_MS) {
    _use_adc = !_use_adc;
    _last_switch_ms = millis();
  }
}

void BrightnessMode::increaseBrightness() {
  detectManualBrightness();
  unsigned int val = _current_brightness + 5;
  _current_brightness = min(val, (unsigned int)255);
}

void BrightnessMode::decreaseBrightness() {
  detectManualBrightness();
  int val = _current_brightness - 5;
  _current_brightness = max(val, 0);
}

void BrightnessMode::reset() { _manual_brightness = false; }

void BrightnessMode::detectManualBrightness() {
  if (!_manual_brightness) {
    _current_brightness = 128;
  }
  _manual_brightness = true;
}