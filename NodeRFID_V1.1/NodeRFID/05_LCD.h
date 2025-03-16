#ifndef LCD_h
#define LCD_h

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

class LCD {
public:
  void khoitao();
  void displayshowADD();
  void displayshowMenu();
  void displayWaitFinger();
  void displayInvalidFinger();
  void displayFingerOK();
  void displayWaitRFID();
  void displayOK();
  void displayNumber(int num, int col, int row);
  void displayClear();
private:

};

#endif
