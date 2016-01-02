#include "VirtualWire.h"
#include "LowPower.h"

#define CMD_SWITCH_OFF 0
#define CMD_SWITCH_ON 1

#define NO_DEBUG

void setup()
{
  #ifndef NO_DEBUG
    Serial.begin(9600);  // Debugging only
    Serial.println("setup");
  #endif

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
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
  
  #ifndef NO_DEBUG
    Serial.println("Sent");
  #endif

  sleep(SLEEP_2S);
}

