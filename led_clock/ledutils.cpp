#include "ledutils.h"

Ledutils::Ledutils(uint16_t matrix_width, uint16_t matrix_height)
    : _matrix_width(matrix_width), _matrix_height(matrix_height) {}

uint16_t Ledutils::XY(uint8_t x, uint8_t y) {
  uint16_t i;

  if (y & 0x01)

  {
    // Odd rows run backwards
    uint8_t reverse_x = (_matrix_width - 1) - x;
    i = (y * _matrix_width) + reverse_x;
  } else {
    // Even rows run forwards
    i = (y * _matrix_width) + x;
  }

  return i;
}

uint8_t Ledutils::getX(uint16_t absolute) {
  uint8_t x = absolute % _matrix_width;
  // Odd rows run backwards
  if (getY(absolute) & 0x01) {
    return _matrix_width - 1 - x;
  }
  return x;
}

uint8_t Ledutils::getY(uint16_t absolute) { return absolute / _matrix_width; }
