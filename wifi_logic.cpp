#include "wifi_logic.h"

WifiLogic::WifiLogic() {
  this->esp = new SoftwareSerial(8, 9);
}

WifiLogic::~WifiLogic() {
  delete esp;
}

void WifiLogic::init() {
  //115200
  Serial.println("WIFI INIT");
  esp->begin(9600);

  for (uint8_t i=0;i<DATA_SIZE;i++) {
    broadcastData[i] = 0;
  }
}

bool WifiLogic::update() {   
  if (isActive && (lastSend==0 || millis() - lastSend > WIFI_CMD_SEND_INTERVAL_MS)) {
    sendCmd(F("OVERRIDE"));
    lastSend = millis();
  }

  if (!isActive && (lastChannelSync==0 || millis() - lastChannelSync > WIFI_CHANNEL_SYNC_INTERVAL_MS)) {
    sendChannelSync(isFirstChannelSync);
    isFirstChannelSync = false;
    lastChannelSync = millis();
  }

  // check bc data
  return updateBroadcastData();
}

void WifiLogic::forceStop() {
  sendCmd(F("STOP"));
  setActive(false);
}

void WifiLogic::setActive(bool isActive) {
  this->isActive = isActive;
  lastSend = 0;    // trigger
}

int WifiLogic::getBroadcastData(uint8_t index) {
  return broadcastData[index];
}

bool WifiLogic::updateBroadcastData() {
  bool returnVal = false;
  
  while (esp->available() > 0) {
    String tmp = esp->readStringUntil('\n');

    if (tmp.startsWith("POST OK")) {
      // post ok
      Serial.println("POST OK");
    } else if (tmp.startsWith("STATE ")) {
      tmp = tmp.substring(6);
      tmp.trim();

      Serial.print("STATE ");
      Serial.println(tmp);
      setBroadcastData(tmp);
      returnVal = true;
    } else if (tmp.startsWith("TEMP ")) {
      tmp = tmp.substring(5);
      tmp.trim();

      Serial.print("TEMP ");
      Serial.println(tmp);
      setBroadcastData(tmp);
      returnVal = true;
    } else {
      Serial.print("Ignoring ");
      Serial.println(tmp);
    }
  }
  
  return returnVal;
}

void WifiLogic::setBroadcastData(String dataString) {
  int pos;
  String val;

  for (uint8_t i = 0;i<DATA_SIZE;i++) {
    pos = dataString.indexOf(',');
    if (pos>0) {
      val = dataString.substring(0,pos);
      dataString = dataString.substring(pos+1);
      
      Serial.print("BROADCAST VALUE ");
      Serial.println(val);
  
      broadcastData[i] = val.toInt();          
    } else {
      // last token
      broadcastData[i] = dataString.toInt();
      Serial.print("LBROADCAST VALUE ");
      Serial.println(broadcastData[i]);
      dataString = "";
    }
  }
}

void WifiLogic::sendCmd(String cmd) {
  Serial.println("Sending cmd " + cmd);

  esp->print(F("HTTP CMD "));
  esp->println(cmd);
}

void WifiLogic::sendChannelSync(bool force) {
  Serial.println("Sending channel sync");
  if (force) {
    esp->println(F("HTTP CHANNEL SYNC FORCEµ"));
  } else {
    esp->println(F("HTTP CHANNEL SYNC"));
  }
}

void WifiLogic::sendForceCheck() {
  Serial.println("Sending forced check");
  esp->println(F("HTTP CHECK FORCE"));
}

