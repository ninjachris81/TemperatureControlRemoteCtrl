#include "connection_logic.h"

ConnectionLogic::ConnectionLogic() {
  conn = new RF24(9,10);
}

ConnectionLogic::~ConnectionLogic() {
  delete conn;
}

ConnectionLogic::BroadcastDataStruct ConnectionLogic::getBroadcastData() {
  return broadcastData;
}

void ConnectionLogic::init() {
  conn->begin();

  conn->openWritingPipe(pipes[1]);
  conn->openReadingPipe(1,pipes[0]);
  conn->setRetries(0,15);
  conn->setPALevel(RF24_PA_MAX);
  conn->enableAckPayload();               // Allow optional ack payloads
  
  conn->startListening();

  conn->printDetails();
}

void ConnectionLogic::setSleepMode(bool value) {
  if (value) {
    conn->powerDown();
  } else {
    conn->powerUp();
  }
}


bool ConnectionLogic::updateIncoming() {
  byte pipeNo;

  if ( conn->available(&pipeNo) >= sizeof(BroadcastDataStruct)){
    conn->read( &broadcastData, sizeof(BroadcastDataStruct) );
    #ifdef IS_DEBUG
      Serial.println(F("SEND ACK"));
    #endif
    conn->writeAckPayload(pipeNo,&cmdData.cmd, sizeof(CmdStruct) );    
    return true;
 } else {
    return false;
 }
}

void ConnectionLogic::setCmd(uint8_t cmd) {
  cmdData.cmd = cmd;
}

