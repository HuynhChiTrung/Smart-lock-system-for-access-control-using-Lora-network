#include "chuongtrinhchinh.h"
void setup() {
  khoitao();
}

void loop() {
  chuongtrinhchinh();
  //dieukhienrelay();
}

// #include <SPI.h>
// #include <MFRC522.h>

// // Khai báo các chân cho RFID
// #define SS_PIN_RFID 14
// #define RST_PIN_RFID 22

// MFRC522 mfrc522(SS_PIN_RFID, RST_PIN_RFID); // Khởi tạo đối tượng MFRC522

// void setup() {
//   Serial.begin(9600);
//   pinMode(16, OUTPUT);
//   digitalWrite(16, 1);
//   Serial.println("RFID Setup");

//   // Khởi tạo MFRC522
//   SPI.begin(4, 18, 19); // Khởi tạo giao tiếp SPI
//   mfrc522.PCD_Init(); // Khởi tạo đầu đọc RFID
//   Serial.println("RFID Reader initialized");
// }

// void loop() {
//   // Kiểm tra nếu có thẻ RFID nào được phát hiện
//   if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//     Serial.print("Card UID: ");
//     String uidStr = "";
//     for (byte i = 0; i < mfrc522.uid.size; i++) {
//       uidStr += String(mfrc522.uid.uidByte[i], HEX);
//       Serial.print(mfrc522.uid.uidByte[i], HEX);
//     }
//     Serial.println();
//     // Ngừng phát hiện thẻ cho đến khi thẻ được rút ra
//     mfrc522.PICC_HaltA();
//   }
// }
