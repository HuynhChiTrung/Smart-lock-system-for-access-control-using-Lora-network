#include "01_SERIAL.h"
#define BAUD_RATE 9600

void MySerial::khoitao(){
    // Thiết lập tốc độ giao tiếp Serial với máy tính
  //Serial.begin(BAUD_RATE);
  
  // Chờ cho kết nối serial sẵn sàng
  while (!Serial) {
    ; // Chờ đến khi Serial kết nối
  }
  
  // Thông báo hệ thống sẵn sàng
  Serial.println("ESP32 đã sẵn sàng. Nhập chuỗi và nhấn Enter:");
}
void MySerial::nhapdulieu(){
  // Kiểm tra xem có dữ liệu nào gửi đến từ serial hay không
  if (Serial.available() > 0) {
    // Đọc chuỗi từ serial
    String inputString = Serial.readStringUntil('\n'); // Đọc cho đến khi gặp ký tự xuống dòng
    
    // Hiển thị lại chuỗi vừa nhập
    Serial.print("Bạn đã nhập: ");
    Serial.println(inputString);
  }
}