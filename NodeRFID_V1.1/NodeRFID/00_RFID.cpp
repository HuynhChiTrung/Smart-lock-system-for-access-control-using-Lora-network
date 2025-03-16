#include "00_RFID.h"
#include "03_KEY.h"

RFID::RFID(int ssPin, int rstPin)
  : mfrc522(ssPin, rstPin) {}

void RFID::khoitao() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  digitalWrite(16, 1);
  Serial.println("RFID Setup");
  // Khởi tạo MFRC522
  SPI.begin(4, 18, 19);  // Khởi tạo giao tiếp SPI
  mfrc522.PCD_Init();    // Khởi tạo đầu đọc RFID
  Serial.println("RFID Reader initialized");
}

String RFID::docthe() {
  String uidStr = "";
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uidStr += String(mfrc522.uid.uidByte[i], DEC);
      Serial.print(mfrc522.uid.uidByte[i], DEC);
    }
    Serial.println();
    // Ngừng phát hiện thẻ cho đến khi thẻ được rút ra
    mfrc522.PICC_HaltA();
  }

  return uidStr;  // Trả về chuỗi UID
}
