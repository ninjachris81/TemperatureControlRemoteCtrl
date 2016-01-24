#ifndef connection_logic_h
#define connection_logic_h

#include <RF24.h>

class ConnectionLogic {
public:
  struct BroadcastDataStruct {
    int tempWater;
    int tempHC;
    int tempTank;
    bool pumpWater;
    bool pumpHC;
  };

  struct CmdStruct {
    uint8_t cmd;
  };


  ConnectionLogic();
  ~ConnectionLogic();

  void init();

  bool updateIncoming();

  void setCmd(uint8_t cmd);

  void setSleepMode(bool value);

  BroadcastDataStruct getBroadcastData();

private:
  const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };

  RF24 *conn;
  
  BroadcastDataStruct broadcastData;

  CmdStruct cmdData;  
};

#endif
