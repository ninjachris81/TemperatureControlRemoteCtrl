#include "display_logic.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #define printByte(args)  write(args);
#else
  #define printByte(args)  print(args,BYTE);
#endif

DisplayLogic::DisplayLogic(int maxTimeoutSec) {
  lcd = new LiquidCrystal_I2C(0x27,16,2);
  this->maxTimeoutSec = maxTimeoutSec;
}

DisplayLogic::~DisplayLogic() {
  delete lcd;
}

void DisplayLogic::init() {
  lcd->init();

  lcd->createChar(CUSTOM_CHAR_INDEX_DEGREE, degree);
  lcd->createChar(CUSTOM_CHAR_INDEX_CONNECTED, isConnected);
  lcd->createChar(CUSTOM_CHAR_INDEX_DISCONNECTED, isDisconnected);

  // print static stuff
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(F("WASSER: "));
 
  lcd->setCursor(0,1);
  lcd->print(F("TAUSCH: "));
}

void DisplayLogic::updateConnected(bool isConnected) {
  lcd->setCursor(15,0);
  if (isConnected) {
    lcd->printByte(CUSTOM_CHAR_INDEX_CONNECTED);
  } else {
    lcd->printByte(CUSTOM_CHAR_INDEX_DISCONNECTED);
  }
}

void DisplayLogic::updateTemp(int tempW, int tempHC, bool pumpWater, bool pumpHC) {

#ifdef IS_DEBUG
  Serial.print(F("TEMPW: "));
  Serial.println(tempW);
  Serial.print(F("TEMPHC: "));
  Serial.println(tempHC);

  Serial.print(F("PUMPW: "));
  Serial.println(pumpWater);
  Serial.print(F("PUMPHC: "));
  Serial.println(pumpHC);
#endif
  
  // update display
  lcd->setCursor(8,0);
  if (tempW<10) lcd->print(" ");
  lcd->print(tempW);
  lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);
  if (pumpHC) {
    lcd->print(F(" AN "));
  } else {
    lcd->print(F(" AUS"));
  }

  lcd->setCursor(8,1);
  if (tempHC<10) lcd->print(" ");
  lcd->print(tempHC);
  lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);
}

void DisplayLogic::updateTimeout(int timeoutSec) {
  if (timeoutSec > 9999) return;
  
#ifdef IS_DEBUG
  Serial.print(F("TIMEOUT: "));
  Serial.println(timeoutSec);
#endif

  lcd->setCursor(12,1);
  if (timeoutSec<1000) lcd->print(" ");
  if (timeoutSec<100) lcd->print(" ");
  if (timeoutSec<10) lcd->print(" ");
  if (timeoutSec==0) {
    lcd->print("  ");
  } else {
    lcd->print(timeoutSec);
  }
}

void DisplayLogic::setActive(bool active) {
  if (active) {
    lcd->backlight();
  } else {
    lcd->noBacklight();
  }
}

