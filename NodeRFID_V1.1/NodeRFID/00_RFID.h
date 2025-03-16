#ifndef _RFID_h
#define _RFID_h

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN_RFID 14
#define RST_PIN_RFID 22

class RFID{
  private:
    MFRC522 mfrc522;
  public:
    RFID(int ssPin = SS_PIN_RFID, int rstPin = RST_PIN_RFID) ; 
    void khoitao();
    String docthe();
};
#endif