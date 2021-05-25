#include <Arduino.h>

#ifndef __LEDUTILS_H__
#define __LEDUTILS_H__

class Ledutils {
public:
  Ledutils(uint16_t matrix_width, uint16_t matrix_height);
  uint16_t XY(uint8_t x, uint8_t y);
  uint8_t getX(uint16_t absolute);
  uint8_t getY(uint16_t absolute);

private:
  uint16_t _matrix_width;
  uint16_t _matrix_height;
};

#endif //__LEDUTILS_H__
