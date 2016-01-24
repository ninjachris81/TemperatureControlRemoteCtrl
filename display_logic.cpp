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

  // print static stuff
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(F("WASSER: "));
 
  lcd->setCursor(0,1);
  lcd->print(F("TAUSCH: "));
}

void DisplayLogic::updateTemp(int tempW, int tempHC, bool pumpWater, bool pumpHC) {

  Serial.print(F("TEMPW: "));
  Serial.println(tempW);
  Serial.print(F("TEMPHC: "));
  Serial.println(tempHC);

  Serial.print(F("PUMPW: "));
  Serial.println(pumpWater);
  Serial.print(F("PUMPHC: "));
  Serial.println(pumpHC);

  
  // update display
  lcd->setCursor(8,0);
  if (tempW<10) lcd->print(" ");
  lcd->print(tempW);
  lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);
  if (pumpHC) {
    lcd->print(F("  ON "));
  } else {
    lcd->print(F("  OFF"));
  }

  lcd->setCursor(8,1);
  if (tempHC<10) lcd->print(" ");
  lcd->print(tempHC);
  lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);
}

void DisplayLogic::updateTimeout(int timeoutSec) {
  if (timeoutSec > 9999) return;
  
  Serial.print(F("TIMEOUT: "));
  Serial.println(timeoutSec);

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

