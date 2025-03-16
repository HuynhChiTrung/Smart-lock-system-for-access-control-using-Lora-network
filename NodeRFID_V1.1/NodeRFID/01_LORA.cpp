#include "01_LORA.h"

#define NODE_ADDRESS 0x01     // Địa chỉ của Node
#define NODE_TG_ADDRESS 0x02  // Địa chỉ của Node
#define MASTER_ADDRESS 0xFF   // Địa chỉ của Master

LORA ::LORA(int SS_LORA)
  : SS_LORA(SS_LORA) {}

void LORA::khoitao() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  SPI.begin(4, 18, 19);
  LoRa.setPins(SS_PIN_LORA, 22, 23);
  while (!LoRa.begin(433E6)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Starting LoRa successfully!");
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
}
void LORA::guidulieu(String dulieu) {
  LoRa.beginPacket();
  LoRa.write(MASTER_ADDRESS);  // Địa chỉ của Master
  //LoRa.write(NODE_TG_ADDRESS);  // Địa chỉ của Master trung gian
  LoRa.write(NODE_ADDRESS);  // Địa chỉ của Node
  //LoRa.print("Card UID: ");
  LoRa.print(dulieu);
  LoRa.endPacket();
  //delay(1000);
}
void LORA::guidulieu(int dulieu) {
  LoRa.beginPacket();
  LoRa.write(MASTER_ADDRESS);  // Địa chỉ của Master
  LoRa.write(NODE_ADDRESS);    // Địa chỉ của Node
  LoRa.print(dulieu);
  LoRa.endPacket();
}
String LORA::nhandulieu() {
  String receivedData = "";
  int packetSize = LoRa.parsePacket();
  // Chuỗi để lưu dữ liệu nhận được  // Kiểm tra xem có gói dữ liệu nào được nhận không
  if (packetSize >= 2) {
    // Đã nhận được gói dữ liệu
    //Serial.print("Received packet '");
    // Đọc gói dữ liệu
    byte recipient = LoRa.read();  // Đọc địa chỉ nhận
    byte sender = LoRa.read();     // Đọc địa chỉ gửi
    if (recipient == NODE_ADDRESS) {
      Serial.print("Received from Node ");
      Serial.print(sender, HEX);
      Serial.print(": '");
      while (LoRa.available()) {
        char receivedChar = (char)LoRa.read();
        receivedData += receivedChar;  // Thêm ký tự nhận được vào chuỗi
        Serial.print(receivedChar);
      }
      Serial.println("'");

      digitalWrite(13, 0);
      delay(300);
      digitalWrite(13, 1);
      delay(300);
      digitalWrite(13, 0);
      delay(300);
      digitalWrite(13, 1);
      // Tính RSSI và SNR sau khi nhận gói tin
      int rssi = LoRa.packetRssi();
      float snr = LoRa.packetSnr();

      Serial.print("RSSI: ");
      Serial.print(rssi);
      Serial.print(", SNR: ");
      Serial.println(snr);
    }
  }
  return receivedData;
}
