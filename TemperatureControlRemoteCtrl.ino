#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RF24.h>
#include "printf.h"

#include "display_logic.h"
#include "connection_logic.h"

//#define IS_DEBUG

#define PIN_SWITCH 6
#define PIN_ECHO 2 // Echo Pin
#define PIN_TRIGGER 3 // Trigger Pin

#define MIN_DISTANCE_DURATION 800

#define BROADCAST_TIMEOUT_MS 10000

#ifdef IS_DEBUG
  #define GLOBAL_TIMEOUT_MS 60000   // 1 min
  #define HIGHLIGHT_TIMEOUT_MS 5000
#else
  #define GLOBAL_TIMEOUT_MS 1200000   // 20 min
  #define HIGHLIGHT_TIMEOUT_MS 10000
#endif

#define CMD_NONE 0
#define CMD_SWITCH_OFF 1
#define CMD_SWITCH_ON 2

DisplayLogic displayLogic(GLOBAL_TIMEOUT_MS / 1000);
ConnectionLogic connectionLogic;

unsigned long startedAtTimeout = 0;
unsigned long highlightTimeout = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastBroadcast = 0;

#define STATE_INIT 0
#define STATE_ENABLED 1
#define STATE_TIMEOUT 2

uint8_t currentState = STATE_INIT;

int buttonState = 0;

void setState(uint8_t state) {
  currentState = state;
  
  switch(currentState) {
    case STATE_INIT:
      #ifdef IS_DEBUG
        Serial.println("STATE INIT");
      #endif
      startedAtTimeout = 0;
      connectionLogic.setCmd(CMD_NONE);
      displayLogic.setActive(false);
      displayLogic.updateConnected(false);
      break;
    case STATE_ENABLED:
      #ifdef IS_DEBUG
        Serial.println("STATE ENABLED");
      #endif
      startedAtTimeout = millis();
      connectionLogic.setCmd(CMD_SWITCH_ON);
      displayLogic.setActive(true);
      break;
    case STATE_TIMEOUT:
      #ifdef IS_DEBUG
        Serial.println("STATE TIMEOUT");
      #endif
      connectionLogic.setCmd(CMD_NONE);
      startedAtTimeout = 0;
      displayLogic.setActive(false);
      break;
  }

  lastDisplayUpdate = 0;
}

bool checkRange() {
  long duration;
  
  digitalWrite(PIN_TRIGGER, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(PIN_TRIGGER, LOW);
  duration = pulseIn(PIN_ECHO, HIGH);
  
  #ifdef IS_DEBUG
  if (duration<MIN_DISTANCE_DURATION) {
    Serial.print("duration: ");
    Serial.println(duration);
  }
  #endif
  
  return duration<MIN_DISTANCE_DURATION;
}

void setup()
{
  #ifdef IS_DEBUG
    Serial.begin(9600);  // Debugging only
    Serial.println("setup");
    printf_begin();
  #endif

  displayLogic.init();

  connectionLogic.init();

   pinMode(PIN_TRIGGER, OUTPUT);
   pinMode(PIN_ECHO, INPUT);

  setState(STATE_INIT);

  pinMode(PIN_SWITCH, INPUT);
  #ifdef IS_DEBUG
    Serial.println("setup complete");
  #endif
}

void loop() {
  if(connectionLogic.updateIncoming()) {
    #ifdef IS_DEBUG
      Serial.println("BROADCAST RECV");
    #endif
    displayLogic.updateTemp(connectionLogic.getBroadcastData().tempWater, connectionLogic.getBroadcastData().tempHC, connectionLogic.getBroadcastData().pumpWater, connectionLogic.getBroadcastData().pumpHC);
    displayLogic.updateConnected(true);
    lastBroadcast = millis();
  } else {
    if (lastBroadcast==0 || millis() - lastBroadcast > BROADCAST_TIMEOUT_MS) {
      displayLogic.updateConnected(false);
    }
  }

  // check switch state
  buttonState = digitalRead(PIN_SWITCH);
  if (buttonState==HIGH) {
    setState(STATE_ENABLED);
  }

  // check timeout
  if (startedAtTimeout!=0 && millis() - startedAtTimeout >= GLOBAL_TIMEOUT_MS) {
    setState(STATE_TIMEOUT);
  }

  if (currentState==STATE_ENABLED && (lastDisplayUpdate==0 || millis()-lastDisplayUpdate > 1000)) {
    displayLogic.updateTimeout((GLOBAL_TIMEOUT_MS - (millis() - startedAtTimeout)) / 1000);
    lastDisplayUpdate = millis();
  }

  if (currentState==STATE_TIMEOUT || currentState==STATE_INIT) {
    if (checkRange()) {
      highlightTimeout = millis();
      displayLogic.setActive(true);
    } else {
      if (highlightTimeout>0 && millis() - highlightTimeout > HIGHLIGHT_TIMEOUT_MS) {
        displayLogic.setActive(false);
        highlightTimeout = 0;
      }
    }
  } else {
    highlightTimeout = 0;
  }
}

