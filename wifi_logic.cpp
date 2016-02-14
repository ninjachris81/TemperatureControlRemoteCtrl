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
  if (isFirstCheck) {
    sendForceCheck();
    isFirstCheck = false;
  }
    
  if (lastSend==0 || millis() - lastSend > WIFI_CMD_SEND_INTERVAL_MS) {
    if (isActive) {
      sendCmd(F("OVERRIDE"));
    }

    lastSend = millis();
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

      int pos;
      String val;
      
      Serial.println(tmp);

      for (uint8_t i = 0;i<DATA_SIZE;i++) {
        pos = tmp.indexOf(',');
        if (pos>0) {
          val = tmp.substring(0,pos);
          tmp = tmp.substring(pos+1);
          
          Serial.print("BROADCAST VALUE ");
          Serial.println(val);

          broadcastData[i] = val.toInt();          
        } else {
          // last token
          broadcastData[i] = tmp.toInt();
          Serial.print("LBROADCAST VALUE ");
          Serial.println(broadcastData[i]);
          tmp = "";
        }
      }
      returnVal = true;
      
    } else {
      Serial.print("Ignoring ");
      Serial.println(tmp);
    }
  }
  
  return returnVal;
}

void WifiLogic::sendCmd(String cmd) {
  Serial.println("Sending cmd " + cmd);

  esp->print(F("HTTP CMD "));
  esp->println(cmd);
}

void WifiLogic::sendForceCheck() {
  Serial.println("Sending forced check");

  esp->println(F("HTTP CHECK FORCE"));
}

