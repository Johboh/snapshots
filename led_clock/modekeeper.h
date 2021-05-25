#include <Arduino.h>

#ifndef __MODEKEEPER_H__
#define __MODEKEEPER_H__

#define ONE_MINUTE_MS 60000

class Modekeeper {
public:
  enum class Mode {
    SNAKE,
    CLOCK,
    RAINBOW,
    BRIGHTNESS,
    SINGLECOLOR,
  };

  enum class Event {
    NONE,
    LEFT,
    RIGHT,
    RESET,
  };

  Modekeeper(Mode default_mode);
  void setMode(Mode mode);
  void emitEvent(Event event);
  void stamp();
  Mode getMode();
  Event getEvent();

private:
  Mode _default_mode;
  Mode _current_mode;
  Event _latest_event;
  unsigned long _last_stamp = 0;
};

#endif //__MODEKEEPER_H__
