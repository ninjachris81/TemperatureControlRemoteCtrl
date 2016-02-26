#include <SoftwareSerial.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
//#include "printf.h"

#include "display_logic.h"
#include "wifi_logic.h"
#include "button.h"

//#define IS_DEBUG

#define PIN_BUTTON1 6
#define PIN_ECHO 2 // Echo Pin
#define PIN_TRIGGER 3 // Trigger Pin

#define MIN_DISTANCE_DURATION 800

#define BROADCAST_TIMEOUT_MS 120000   // 2 min

#define STARTING_TIMEOUT_MS 30000

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
WifiLogic wifiLogic;
Button button1;

unsigned long startedAtTimeout = 0;
unsigned long highlightTimeout = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastBroadcast = 0;
unsigned long startingTimout = 0;

#define STATE_INIT 0
#define STATE_STARTING 1
#define STATE_ENABLED 2
#define STATE_TIMEOUT 3
#define STATE_STOPPED 4

uint8_t currentState = STATE_INIT;

uint8_t button1State;

void setState(uint8_t state) {
  switch(state) {
    case STATE_INIT:
      #ifdef IS_DEBUG
        Serial.println("STATE INIT");
      #endif
      startedAtTimeout = 0;
      wifiLogic.setActive(false);
      displayLogic.setActive(true);
      break;
    case STATE_STARTING:
      #ifdef IS_DEBUG
        Serial.println("STATE STARTING");
      #endif
      wifiLogic.setActive(true);
      displayLogic.setActive(true);
      displayLogic.printStarting();
      startingTimout = millis();
      break;
    case STATE_ENABLED:
      if (currentState==STATE_STARTING) {
        // ok
        startedAtTimeout = millis();
        startingTimout = 0;
        #ifdef IS_DEBUG
          Serial.println("STATE ENABLED");
        #endif
      } else {
        state = currentState;
      }
      break;
    case STATE_TIMEOUT:
      #ifdef IS_DEBUG
        Serial.println("STATE TIMEOUT");
      #endif
      wifiLogic.setActive(false);
      startedAtTimeout = 0;
      displayLogic.updateTimeout(0);
      displayLogic.setActive(false);
      break;
    case STATE_STOPPED:
      #ifdef IS_DEBUG
        Serial.println("STATE STOPPED");
      #endif
      wifiLogic.forceStop();
      displayLogic.updateTimeout(0);
      startedAtTimeout = 0;
      highlightTimeout = millis();
      break;
  }

  currentState = state;
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
  if (duration>0 && duration<MIN_DISTANCE_DURATION) {
    //Serial.print("duration: ");
    //Serial.println(duration);
  }
  #endif
  
  return duration<MIN_DISTANCE_DURATION;
}

void setup()
{
  #ifdef IS_DEBUG
    Serial.begin(115200);  // Debugging only
    Serial.println("setup");
    //printf_begin();
  #endif

  button1.init(PIN_BUTTON1);

  wifiLogic.init();

  displayLogic.init();

  delay(3000);     // wait for esp

  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  setState(STATE_INIT);
  
  #ifdef IS_DEBUG
  Serial.println("setup complete");
  #endif
}

void loop() {
  if(wifiLogic.update()) {
    #ifdef IS_DEBUG
      Serial.println("BROADCAST RECV");
    #endif
    displayLogic.updateTemp(wifiLogic.getBroadcastData(FIELD_INDEX_WATER), wifiLogic.getBroadcastData(FIELD_INDEX_HC), wifiLogic.getBroadcastData(FIELD_INDEX_TANK), wifiLogic.getBroadcastData(FIELD_INDEX_PUMP_WATER_ON), wifiLogic.getBroadcastData(FIELD_INDEX_PUMP_HC_ON), wifiLogic.isConnected());
    lastBroadcast = millis();
    setState(STATE_ENABLED);
  } else {
    if (lastBroadcast>0 && millis() - lastBroadcast > BROADCAST_TIMEOUT_MS) {
      #ifdef IS_DEBUG
        Serial.println("BROADCAST TIMEOUT");
      #endif
      displayLogic.updateTemp(-1, -1, -1, false, false, wifiLogic.isConnected());
      lastBroadcast = 0;
    }
  }

  if (currentState==STATE_STARTING && startingTimout>0 && millis() - startingTimout > STARTING_TIMEOUT_MS) {
    Serial.println("STARTING TIMEOUT");
    setState(STATE_TIMEOUT);
    startingTimout = 0;
  }

  // check switch state
  button1State = button1.update();
  if (button1State==BUTTON_EV_LONG_PRESS) {
    setState(STATE_STOPPED);
  } else if (button1State==BUTTON_EV_PRESS) {
    setState(STATE_STARTING);
  }

  // check timeout
  if (startedAtTimeout!=0 && millis() - startedAtTimeout >= GLOBAL_TIMEOUT_MS) {
    setState(STATE_TIMEOUT);
  }

  if (currentState==STATE_ENABLED && (lastDisplayUpdate==0 || millis()-lastDisplayUpdate > 1000)) {
    displayLogic.updateTimeout((GLOBAL_TIMEOUT_MS - (millis() - startedAtTimeout)) / 1000);
    lastDisplayUpdate = millis();
  }

  if (currentState==STATE_TIMEOUT || currentState==STATE_INIT || currentState==STATE_STOPPED) {
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

