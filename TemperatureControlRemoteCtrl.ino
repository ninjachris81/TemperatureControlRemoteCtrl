#include "VirtualWire.h"
#include "LowPower.h"

#define CMD_SWITCH_OFF 0
#define CMD_SWITCH_ON 1

//#define IS_DEBUG

#define PIN_LED 13

#define GLOBAL_TIMEOUT_MS 1200000   // 20 min
#define GLOBAL_TIMEOUT_MS_TEST 60000   // 1 min

void setup()
{
  #ifdef IS_DEBUG
    Serial.begin(9600);  // Debugging only
    Serial.println("setup");
  #endif

  pinMode(PIN_LED, OUTPUT);

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
  
  digitalWrite(PIN_LED, HIGH);
}

void sleep(period_t period) {
  LowPower.powerDown(period, ADC_OFF, BOD_OFF); 
}

void loop()
{
  char *msg = " ";
  msg[0] = CMD_SWITCH_ON;
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx(); // Wait until the whole message is gone
  
  #ifdef IS_DEBUG
    Serial.println("Sent");
  #endif

  if (millis() < GLOBAL_TIMEOUT_MS_TEST) {
    #ifdef IS_DEBUG
      delay(2000);
      Serial.println("Sent");
    #else
      sleep(SLEEP_2S);
    #endif
  } else {
    #ifdef IS_DEBUG
      Serial.println("TIMEOUT");
    #endif
    digitalWrite(PIN_LED, LOW);
    sleep(SLEEP_FOREVER);  
  }
}

