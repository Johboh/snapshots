#include <Arduino.h>

#ifndef __BRIGHTNESS_H__
#define __BRIGHTNESS_H__

class Brightness {
public:
  void handle();
  uint8_t getBrightness();
  unsigned int getLatestAdcValue();
  bool saturated();

private:
  unsigned int readAdc();

private:
  uint8_t _brightness;
  unsigned int _latest_adc;
};

#endif //__BRIGHTNESS_H__
