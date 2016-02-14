#ifndef wifi_logic_h
#define wifi_logic_h

#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CMD_NONE 0
#define CMD_SWITCH_ON 1

#define WIFI_CMD_SEND_INTERVAL_MS 10000

// @see MainUnit->http_logic.h->executeData()
#define FIELD_INDEX_WATER 0
#define FIELD_INDEX_HC 1
#define FIELD_INDEX_TANK 2
#define FIELD_INDEX_PUMP_WATER_ON 3
#define FIELD_INDEX_PUMP_HC_ON 4
#define FIELD_INDEX_TS 5

#define DATA_SIZE 6

class WifiLogic {
public:
  WifiLogic();
  ~WifiLogic();

  void init();

  bool update();

  int getBroadcastData(uint8_t index);

  void setActive(bool isActive);

  void forceStop();

private:
  SoftwareSerial *esp;
  unsigned long lastSend = 0;
  bool isActive = false;

  bool isFirstCheck = true;

  int broadcastData[DATA_SIZE];

  void sendCmd(String cmd);

  void sendForceCheck();

  bool updateBroadcastData();

};

#endif
