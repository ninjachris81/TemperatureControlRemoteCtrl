#ifndef BUTTON_H
#define BUTTON_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define BUTTON_EV_NONE 0
#define BUTTON_EV_PRESS 1
#define BUTTON_EV_LONG_PRESS 2

#define BUTTON_LONG_PRESS_MS 4000

class Button {
public:
  void init(uint8_t pin);

  uint8_t update();

private:
  uint8_t pin = 0;

  int buttonState;
  int lastButtonState = LOW;

  unsigned long buttonHighStartedAt = 0;
};


#endif
