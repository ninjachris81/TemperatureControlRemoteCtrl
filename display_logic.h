#ifndef display_logic_h
#define display_logic_h

#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CUSTOM_CHAR_INDEX_DEGREE 0

class DisplayLogic {
public:
  DisplayLogic(int maxTimeoutSec);
  ~DisplayLogic();

  void init();

  void updateTemp(int tempW, int tempHC, bool pumpWater, bool pumpHC);

  void updateTimeout(int timeoutSec);

  void setActive(bool active);

private:
  LiquidCrystal_I2C *lcd;
  int maxTimeoutSec;
  uint8_t degree[8] = {  0xe,0x11,0x11,0xe,0x0,0x0,0x0};

};

#endif
