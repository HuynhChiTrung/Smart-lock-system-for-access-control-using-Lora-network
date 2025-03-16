#include "00_LORA.h"
#define MASTER_ADDRESS 0xFF  // Địa chỉ của Master
#define NODE_ADDRESS 0x01    // Địa chỉ của Master

void LORA::khoitao() {
  Serial.begin(115200);
  //Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
  SPI.begin(4, 18, 19, 21);
  LoRa.setPins(21, 22, 23);
  Serial.println("LoRa Receiver");

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
// void LORA::guidulieu() {
//   LoRa.beginPacket();
//   //LoRa.write(NODE_ADDRESS);    // Địa chỉ của Node
//   LoRa.write(sender);
//   LoRa.write(MASTER_ADDRESS);  // Địa chỉ của Master
//   LoRa.print("ok");
//   LoRa.endPacket();
//   delay(1000);
// }
void LORA::guidulieu(String dulieu) {
  LoRa.beginPacket();
  LoRa.write(NODE_ADDRESS);  // Địa chỉ của Node
  //LoRa.write(sender);
  LoRa.write(MASTER_ADDRESS);  // Địa chỉ của Master
  LoRa.print(dulieu);
  LoRa.endPacket();
  //delay(1000);
}
String LORA::nhandulieu() {
  int packetSize = LoRa.parsePacket();
  String receivedData = "";  // Chuỗi để lưu dữ liệu nhận được  // Kiểm tra xem có gói dữ liệu nào được nhận không
  if (packetSize > 0) {
    // Đã nhận được gói dữ liệu
    //Serial.print("Received packet '");
    // Đọc gói dữ liệu
    byte recipient = LoRa.read();  // Đọc địa chỉ nhận
    sender = LoRa.read();          // Đọc địa chỉ gửi
    if (recipient == MASTER_ADDRESS) {
      Serial.print("Received from Node ");
      Serial.print(sender, HEX);
      Serial.print(": '");
      while (LoRa.available()) {
        char receivedChar = (char)LoRa.read();
        receivedData += receivedChar;  // Thêm ký tự nhận được vào chuỗi
        Serial.print(receivedChar);
      }
      Serial.println("'");
      // Tính RSSI và SNR sau khi nhận gói tin
      int rssi = LoRa.packetRssi();
      float snr = LoRa.packetSnr();

      Serial.print("RSSI: ");
      Serial.print(rssi);
      Serial.print(", SNR: ");
      Serial.print(snr);
      Serial.println();
    }
  }
  return receivedData;
}
