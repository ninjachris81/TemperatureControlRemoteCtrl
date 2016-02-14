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
  Serial.println("DISPLAY INIT");

  lcd->init();

  lcd->clear();
  lcd->createChar(CUSTOM_CHAR_INDEX_DEGREE, degree);

  printHomeScreen();
}

void DisplayLogic::printHomeScreen() {
  // print static stuff
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(F("TANK  WT  WASSER"));
 
  lcd->setCursor(0,1);
  lcd->print(F("   VERBINDE..."));
  wasHomescreen = true;
}

void DisplayLogic::printStarting() {
  lcd->setCursor(0,1);
  lcd->print(F("STARTE"));
}

void DisplayLogic::updateTemp(int tempWater, int tempHC, int tempTank, bool pumpWater, bool pumpHC) {

#ifdef IS_DEBUG
  Serial.print(F("TEMPW: "));
  Serial.println(tempWater);
  Serial.print(F("TEMPHC: "));
  Serial.println(tempHC);

  Serial.print(F("PUMPW: "));
  Serial.println(pumpWater);
  Serial.print(F("PUMPHC: "));
  Serial.println(pumpHC);
#endif

  if (wasHomescreen) {
    lcd->clear();
    wasHomescreen = false;
  }
  
  // update display
  if (tempWater==-1 && tempHC==-1) {
    /*
    lcd->setCursor(8,0);
    lcd->print("      ");
    lcd->setCursor(8,1);
    lcd->print("      ");
    */
  } else {
    lcd->setCursor(0,0);
    
    lcd->print("  ");
    printNumber2(tempTank);
    lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);
  
    lcd->print("  ");
    printNumber2(tempHC);
    lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);

    lcd->print("  ");
    printNumber2(tempWater);
    lcd->printByte(CUSTOM_CHAR_INDEX_DEGREE);

    lcd->setCursor(7,1);
    lcd->print(pumpHC ? F("AUS") : F("AN "));
    lcd->print("  ");
    lcd->print(pumpWater ? F("AUS") : F("AN "));
  }
}

void DisplayLogic::printNumber2(uint8_t number) {
  if (number<10) lcd->print(" ");
  lcd->print(number);
}

void DisplayLogic::printNumber4(uint16_t number, bool printZero) {
  if (number<1000) lcd->print(" ");
  if (number<100) lcd->print(" ");
  if (number<10) lcd->print(" ");
  if (number==0 && !printZero) {
    lcd->print(" ");
  } else {
    lcd->print(number);
  }
}

void DisplayLogic::updateTimeout(int timeoutSec) {
  if (timeoutSec > 9999) return;
  
#ifdef IS_DEBUG
  Serial.print(F("TIMEOUT: "));
  Serial.println(timeoutSec);
#endif

  lcd->setCursor(0,1);
  if (timeoutSec==0) {
    lcd->print("      ");
  } else {
    printNumber4(timeoutSec, false);
    lcd->print("s ");
  }
}

void DisplayLogic::setActive(bool active) {
  if (active) {
    lcd->backlight();
  } else {
    lcd->noBacklight();
  }
}

