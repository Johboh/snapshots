#include "snake.h"

Snake::Snake(uint16_t matrix_width, uint16_t matrix_height, Ledutils &ledutils)
    : _matrix_width(matrix_width), _matrix_height(matrix_height), _num_leds(matrix_width * matrix_height),
      _ledutils(ledutils), _font(Font(RainbowColors_p, _ledutils)) {
  reset();
  _font.setBrightness(255);
}

void Snake::reset() {
  _tick_delay_ms = 500;
  _length = 2;
  _body.clear();
  _body.add(_ledutils.XY(1, 0)); // First
  _body.add(_ledutils.XY(0, 0));
  _direction = Direction::RIGHT;
  _state = State::PLAYING;
  randomSeed(analogRead(0)); // Not so random, based on brightness.
  _food = getNewFood();
  _next_update = millis() + _tick_delay_ms;
  _rotations.clear();
}

void Snake::rotateLeft() { _rotations.add(0, Rotation::LEFT); }
void Snake::rotateRight() { _rotations.add(0, Rotation::RIGHT); }

void Snake::updateDirectionFromRotation() {
  // Take next one at the back, if any.
  if (_rotations.size() >= 1) {
    Rotation rotation = _rotations.pop();

    if (rotation == Rotation::LEFT) {
      // Use ++/-- instead.
      switch (_direction) {
      case Direction::LEFT:
        _direction = Direction::DOWN;
        break;
      case Direction::RIGHT:
        _direction = Direction::UP;
        break;
      case Direction::UP:
        _direction = Direction::LEFT;
        break;
      case Direction::DOWN:
        _direction = Direction::RIGHT;
        break;
      }
    } else if (rotation == Rotation::RIGHT) {
      // Use ++/-- instead.
      switch (_direction) {
      case Direction::LEFT:
        _direction = Direction::UP;
        break;
      case Direction::RIGHT:
        _direction = Direction::DOWN;
        break;
      case Direction::UP:
        _direction = Direction::RIGHT;
        break;
      case Direction::DOWN:
        _direction = Direction::LEFT;
        break;
      }
    }
  }
}

int16_t Snake::getNewHead() {
  uint16_t current_head = _body.get(0);
  // Given direction, calculate new head.
  uint8_t x = _ledutils.getX(current_head);
  uint8_t y = _ledutils.getY(current_head);
  updateDirectionFromRotation();
  switch (_direction) {
  case Direction::LEFT:
    --x;
    break;
  case Direction::RIGHT:
    ++x;
    break;
  case Direction::UP:
    ++y;
    break;
  case Direction::DOWN:
    --y;
    break;
  }
  // In case of negative out of bounds, x/y will overflow.
  if (x >= _matrix_width || y >= _matrix_height) {
    return -1;
  }

  uint16_t new_head = _ledutils.XY(x, y);
  // Check if eating itself.
  for (uint16_t i = 0; i < _body.size(); ++i) {
    if (_body.get(i) == new_head) {
      // Self consumption.
      return -1;
    }
  }

  return new_head;
}

uint16_t Snake::getNewFood() {
  // Quite expensive!
  while (true) {
    long rand_led = random(_num_leds);
    bool any_body_match = false;
    for (uint16_t i = 0; i < _body.size(); ++i) {
      if (_body.get(i) == rand_led) {
        any_body_match = true;
        break;
      }
    }
    if (!any_body_match) {
      return rand_led;
    }

    yield(); // Don't get caught by watchdog
  }
  // Will never get here
  return 0;
}

void Snake::gameplay(CRGB *leds) {
  if (millis() >= _next_update) {
    // Add new item first (based on previous first).
    int16_t new_head = getNewHead();
    if (new_head == -1) {
      // Dead in wall or in one self.
      _state = State::DEAD;
      return;
    }
    _body.add(0, new_head); // Add first

    // Ate food?
    if (_food == new_head) {
      _food = getNewFood();
      ++_length;
    }

    while (_body.size() > _length) {
      _body.pop(); // Remove last
    }

    _next_update = millis() + _tick_delay_ms;
  }

  for (uint16_t i = 0; i < _body.size(); ++i) {
    leds[_body.get(i)] = CRGB::Green;
  }
  leds[_food] = CRGB::Red;
}

void Snake::dead(CRGB *leds) {

  // Digits
  char buf[4];
  sprintf(buf, "%03d", _length);
  _font.drawDigit(leds, buf[0] - 48, 7);
  _font.drawDigit(leds, buf[1] - 48, 12);
  _font.drawDigit(leds, buf[2] - 48, 17);

  // Sad face
  leds[_ledutils.XY(0, 0)] = CRGB::Blue;
  leds[_ledutils.XY(1, 1)] = CRGB::Blue;
  leds[_ledutils.XY(2, 1)] = CRGB::Blue;
  leds[_ledutils.XY(3, 1)] = CRGB::Blue;
  leds[_ledutils.XY(4, 1)] = CRGB::Blue;
  leds[_ledutils.XY(5, 0)] = CRGB::Blue;
  leds[_ledutils.XY(1, 3)] = CRGB::Blue;
  leds[_ledutils.XY(4, 3)] = CRGB::Blue;
}

void Snake::handle(CRGB *leds) {
  for (uint16_t i = 0; i < _num_leds; ++i) {
    leds[i] = CRGB::Black;
  }

  if (_state == State::PLAYING) {
    gameplay(leds);
  } else if (_state == State::DEAD) {
    dead(leds);
  }
}
