#ifndef _LORA_h
#define _LORA_h

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

class LORA{
  private:
    byte sender;  // Biến để lưu địa chỉ của Node gửi
  public:
    void khoitao();
  //  void guidulieu();
    void guidulieu(String dulieu);
    String nhandulieu();
};
#endif