#include "button.h"

void Button::init(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

uint8_t Button::update() {
  uint8_t returnVal = BUTTON_EV_NONE;
  
  if (pin>0) {

    buttonState = digitalRead(pin);
    
    if (buttonState==HIGH) {
      if (buttonHighStartedAt==0 && buttonState!=lastButtonState) {
        buttonHighStartedAt = millis();
      } else if (buttonHighStartedAt>0 && millis() - buttonHighStartedAt > BUTTON_LONG_PRESS_MS) {
        buttonHighStartedAt = 0;
        returnVal = BUTTON_EV_LONG_PRESS;
      }
    } else if (buttonHighStartedAt>0 && buttonState!=lastButtonState) {
      buttonHighStartedAt = 0;
      returnVal = BUTTON_EV_PRESS;
    }

    lastButtonState = buttonState;
  }

  return returnVal;
}

