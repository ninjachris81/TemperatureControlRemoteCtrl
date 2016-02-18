#ifndef display_logic_h
#define display_logic_h

#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CUSTOM_CHAR_INDEX_DEGREE 0

#define TANK_DEGREE_PLUS 10

class DisplayLogic {
public:
  DisplayLogic(int maxTimeoutSec);
  ~DisplayLogic();

  void init();

  void updateTemp(int tempWater, int tempHC, int tempTank, bool pumpWater, bool pumpHC);

  void updateTimeout(int timeoutSec);

  void printStarting();

  void setActive(bool active);

private:
  LiquidCrystal_I2C *lcd;
  int maxTimeoutSec;

  bool wasHomescreen = true;
  
  uint8_t degree[8] = {  0xe,0x11,0x11,0xe,0x0,0x0,0x0};
  //uint8_t isConnected[8] = {  0x0,0x4,0xe,0x1f,0xe,0x4,0x0};
  //uint8_t isDisconnected[8] = {  0x4,0xe,0x1f,0x0,0x1f,0xe,0x4};

  void printHomeScreen();

  void printNumber2(uint8_t number);
  void printNumber4(uint16_t number, bool printZero);

};

#endif
