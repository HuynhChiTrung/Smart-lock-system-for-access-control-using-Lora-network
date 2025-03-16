// #include "00_LORA.h"
// #include "chuongtrinhchinh.h"

// LORA lora;

// void khoitao() {
//   Serial.begin(9600);
//   lora.khoitao();
//   pinMode(13, OUTPUT);
//   digitalWrite(13, 1);
// }

// void chuongtrinhchinh() {
//   String receivedData = lora.nhandulieu();
//   if (receivedData!= "") {
//     Serial.println();
//     //delay(150);
//    // lora.guidulieu();
//     digitalWrite(13, 0);
//     delay(200);
//     digitalWrite(13, 1);
//   }
//   // else {
//   //   if(receivedData.length() > 0){
//   //     Serial.println();
//   //     Serial.println("Invalid card");
//   //   }
//   //}
// }
#include "00_LORA.h"  // Thư viện cho LoRa
#include "01_SERIAL.h"
#include "chuongtrinhchinh.h"  // Chương trình chính
#include <Arduino.h>
#include "wifi.h"
#include "webConfig.h"

LORA lora;
WiFiManager wifiManager;

// String passcode = "";      // Chuyển đổi sang kiểu String
// String receivedData = "";  // Không gửi RFID
// String fingerprint = "";   // Không gửi vân tay

void khoitao() {

  // Gọi sendCredentials với chuỗi kiểu C

  Serial.begin(115200);
  lora.khoitao();
  wifiManager.begin();

  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
}

bool dataSent = false;           // Cờ để kiểm tra xem dữ liệu đã được gửi chưa
bool dataSent1 = false;          // Cờ để kiểm tra xem dữ liệu đã được gửi chưa
String previousDataToSend = "";  // Lưu trữ dữ liệu trước đó

void chuongtrinhchinh() {
  wifiManager.checkButton();        // Kiểm tra trạng thái nút BOOT
  wifiManager.scheduleDataFetch();  // Gọi hàm fetch dữ liệu
  wifiManager.checkWiFiStatusAndUpdateLED();
  //delay(20);

  String dataToSend = wifiManager.sendRFIDAndPasscodeOverLoRa();
  // Serial.println("Data to send: " + dataToSend);
  if (dataToSend != "" && dataToSend != previousDataToSend) {  // Kiểm tra dữ liệu mới và khác dữ liệu trước
    Serial.println("Data to send: " + dataToSend);             // Hiển thị dữ liệu để gửi
    delay(300);
    lora.guidulieu(dataToSend);  // Gửi dữ liệu qua LoRa
    previousDataToSend = dataToSend;  // Cập nhật dữ liệu trước
  }
  
  String receivedData = lora.nhandulieu();
  if (receivedData != "" && !dataSent) {  // Nếu có dữ liệu mới và chưa gửi
    //   Serial.println("Dữ liệu nhận được: " + receivedData);
    //   digitalWrite(13, 0);
    //   delay(200);
    //   digitalWrite(13, 1);
    //   delay(200);
    //   digitalWrite(13, 0);
    //   delay(200);
    //   digitalWrite(13, 1);

    //   // Gửi dữ liệu
    //   wifiManager.sendCredentials(passcode, receivedData, fingerprint);

    //   dataSent = true;  // Đánh dấu là dữ liệu đã được gửi
    // }
    // else if (receivedData == "") {
    //   dataSent = false;  // Nếu không có dữ liệu, cho phép gửi lại khi có dữ liệu mới
    receiveDataLoRa(receivedData);
  }
}
void receiveDataLoRa(String receivedData) {
  String rfid = "";
  String passcode = "";
  int fingerprintID = -1;

  // Phân tích chuỗi nhận được
  int rfidPos = receivedData.indexOf("rfid=");
  if (rfidPos != -1) {
    int rfidEndPos = receivedData.indexOf(";", rfidPos);
    rfid = receivedData.substring(rfidPos + 5, rfidEndPos);  // Lấy giá trị rfid
  }

  int passcodePos = receivedData.indexOf("pass=");
  if (passcodePos != -1) {
    int passcodeEndPos = receivedData.indexOf(";", passcodePos);
    passcode = receivedData.substring(passcodePos + 5, passcodeEndPos);  // Lấy giá trị passcode
  }

  int fingerprintPos = receivedData.indexOf("fingerprintID=");
  if (fingerprintPos != -1) {
    int fingerprintEndPos = receivedData.indexOf(";", fingerprintPos);
    fingerprintID = receivedData.substring(fingerprintPos + 14, fingerprintEndPos).toInt();  // Lấy giá trị fingerprintID
  }
  wifiManager.sendCredentials(passcode, rfid, String(fingerprintID));
  // Sau khi phân tích, bạn có thể sử dụng các giá trị này cho các mục đích khác
  Serial.println("RFID: " + rfid);
  Serial.println("Passcode: " + passcode);
  Serial.println("Fingerprint ID: " + String(fingerprintID));
}