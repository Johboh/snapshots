#include "FastLED.h"
#include "font.h"
#include "ledutils.h"
#include <Arduino.h>
#include <LinkedList.h>

#ifndef __SNAKE_H__
#define __SNAKE_H__

class Snake {
public:
  Snake(uint16_t matrix_width, uint16_t matrix_height, Ledutils &ledutils);
  void handle(CRGB *leds);
  void reset();
  void rotateLeft();
  void rotateRight();

private:
  int16_t getNewHead();
  void gameplay(CRGB *leds);
  void dead(CRGB *leds);
  uint16_t getNewFood();
  void updateDirectionFromRotation();

private:
  enum class Direction { UP, RIGHT, DOWN, LEFT };

  enum class Rotation {
    LEFT,
    RIGHT,
  };

  enum class State {
    PLAYING,
    DEAD,
  };

  Ledutils &_ledutils;
  Font _font;
  uint16_t _num_leds;
  uint16_t _matrix_width;
  uint16_t _matrix_height;
  uint16_t _food;
  unsigned long _tick_delay_ms;
  unsigned long _next_update;
  uint16_t _length;
  State _state;
  Direction _direction;
  LinkedList<uint16_t> _body;
  LinkedList<Rotation> _rotations;
};

#endif //__SNAKE_H__
