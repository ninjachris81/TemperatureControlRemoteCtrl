#ifndef wifi_logic_h
#define wifi_logic_h

#include <SoftwareSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define WIFI_CMD_SEND_INTERVAL_MS 10000
#define WIFI_CHANNEL_SYNC_INTERVAL_MS 60000

// @see MainUnit->http_logic.h->executeData()
#define FIELD_INDEX_WATER 0
#define FIELD_INDEX_HC 1
#define FIELD_INDEX_TANK 2
#define FIELD_INDEX_PUMP_WATER_ON 3
#define FIELD_INDEX_PUMP_HC_ON 4
#define FIELD_INDEX_TS 5

#define DATA_SIZE 6

#define PIN_RX 8
#define PIN_TX 7

class WifiLogic {
public:
  WifiLogic();
  ~WifiLogic();

  void init();

  bool update();

  int getBroadcastData(uint8_t index);

  void setActive(bool isActive);

  void forceStop();

  bool isConnected();

private:
  SoftwareSerial *esp;
  unsigned long lastSend = 0;
  unsigned long lastChannelSync = 0;
  bool isActive = false;

  bool _isConnected = false;

  bool isFirstChannelSync = true;

  int broadcastData[DATA_SIZE];

  void sendCmd(String cmd);

  void sendForceCheck();

  void sendChannelSync(bool force);

  bool updateBroadcastData();

  void setBroadcastData(String dataString);

};

#endif
