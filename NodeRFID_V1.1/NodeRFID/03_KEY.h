#ifndef KeypadController_h
#define KeypadController_h

#include <Adafruit_MPR121.h>

class KEY {
public:
  //KeypadController(uint8_t lcdAddress, uint8_t mpr121Address, uint8_t piezoPin);
  void khoitao();
  String dockey();
  String passcode;
  void resetPasscode();
  bool kiemtrapasscode(const String &inputPasscode);
  void bip(int lan, int tanso, int volume, int time);
  void bipkey(int tanso, int volume, int time);
  String getKeyPress();
  String nhapid();
private:

  void displayKey();
};

#endif
