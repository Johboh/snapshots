#include "brightness.h"

/**
 * y=kx+m
 * y, brightness(0-255)
 * x, adc(0-1023)
 * 30=k30+m
 * 255=k700+m
 * full white/saturated if adc=750+
 */
const double k_value = 225.0 / 670.0;
const double m_value = 30 - 30 * k_value;

unsigned int Brightness::readAdc() {
  int val = 0;
  for (uint8_t i = 0; i < 5; i++) {
    val += analogRead(A0);
    delayMicroseconds(100);
  }
  return val / 5;
}

void Brightness::handle() {
  _latest_adc = readAdc();
  unsigned int calculated_brightness = k_value * _latest_adc + m_value;
  _brightness = min((int)calculated_brightness, 255);
  _brightness = max((int)_brightness, 25);
}

uint8_t Brightness::getBrightness() { return _brightness; }

unsigned int Brightness::getLatestAdcValue() { return _latest_adc; }

bool Brightness::saturated() { return _latest_adc >= 750; }
