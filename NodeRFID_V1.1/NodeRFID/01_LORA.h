#ifndef _LORA_h
#define _LORA_h

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <MFRC522.h>

#define SS_PIN_LORA 21

class LORA{
  private:
    int SS_LORA;
  public:
    LORA(int SS_LORA = SS_PIN_LORA);
    void khoitao();
    void guidulieu(String dulieu);
    void guidulieu(int dulieu);
    String nhandulieu();
};
#endif