#include "03_KEY.h"
#include "05_LCD.h"
// Địa chỉ I2C của MPR121 (địa chỉ mặc định là 0x5A)
Adafruit_MPR121 mpr121 = Adafruit_MPR121();

// Khai báo địa chỉ và khởi tạo LCD (Địa chỉ LCD phổ biến là 0x27 hoặc 0x3F)

uint16_t lastTouched = 0;
const char keyMap[12] = { '*', '7', '4', '1', '0', '8', '5', '2', '#', '9', '6', '3' };
int piezoPin = 12;   // Chân GPIO kết nối với piezo
int ledChannel = 0;  // Kênh LEDC

// KeypadController::KeypadController(uint8_t lcdAddress, uint8_t mpr121Address, uint8_t piezoPin)
//   : lcd(lcdAddress, 16, 2), piezoPin(piezoPin), ledChannel(0), lastTouched(0) {
// }

void KEY::khoitao() {
  Serial.begin(115200);
  Wire.begin(33, 32);  // GPIO 22 là SDA và GPIO 23 là SCL

  // Kiểm tra kết nối với MPR121
  if (!mpr121.begin(0x5A)) {
    Serial.println("Không thể tìm thấy MPR121, kiểm tra kết nối!");
    // while (1)
    //   ;
  } else {
    Serial.println("MPR121 đã sẵn sàng!");
  };
  mpr121.setThresholds(10, 6);  // Tăng ngưỡng touch lên 12 và ngưỡng release xuống 6


  // Cấu hình kênh LEDC với độ phân giải 13-bit và tần số khởi tạo
  ledcSetup(ledChannel, 5000, 13);
  // Gán kênh LEDC với chân GPIO của Piezo
  ledcAttachPin(piezoPin, ledChannel);
}

void KEY::bip(int lan, int tanso, int volume, int time) {
  for (int i = 0; i < lan; i++) {
    delay(time);
    ledcWriteTone(ledChannel, tanso);
    ledcWrite(ledChannel, volume);  // Giảm âm lượng
    delay(time);
    ledcWrite(ledChannel, 0);  // Tắt âm thanh
  }
}
void KEY::bipkey(int tanso, int volume, int time) {
  ledcWriteTone(ledChannel, tanso);
  ledcWrite(ledChannel, volume);  // Giảm âm lượng
  delay(time);
  ledcWrite(ledChannel, 0);  // Tắt âm thanh
}


void KEY::displayKey() {
  lcd.setCursor(0, 0);
  lcd.print(" Hay nhap PASS! ");
  lcd.setCursor(0, 1);
  lcd.print("pass:           ");
  lcd.setCursor(9, 1);
  // Tạo chuỗi dấu '*' với số lượng ký tự bằng với độ dài của passcode
  String maskedPasscode = "";
  for (size_t i = 0; i < passcode.length(); i++) {
    maskedPasscode += '*';
  }
  // Hiển thị chuỗi dấu '*'
  lcd.print(maskedPasscode);
}
void KEY::resetPasscode() {
  passcode = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Hay nhap PASS! ");
  delay(100);
}
// bool KEY::kiemtrapasscode(const String &inputPasscode) {
//   for (const String &validPasscode : validPasscodes) {
//     if (inputPasscode == validPasscode) {
//       return true;
//     }
//   }
//   return false;
// }

// void KEY::update() {
//   uint16_t currentTouched = mpr121.touched();

//   for (uint8_t i = 0; i < 12; i++) {
//     if ((currentTouched & (1 << i)) && !(lastTouched & (1 << i))) {

//       char key = keyMap[i];
//       // Serial.print("Nút ");
//       // Serial.print(key);
//       // Serial.println(" đã được chạm!");

//       if (key == '#') {
//         resetPasscode();
//       } else if (passcode.length() < 6) {
//         passcode += key;
//         displayKey();
//         bip(1, 5000, 20, 100);

//         // Kiểm tra nếu passcode đạt 6 ký tự
//         if (passcode.length() == 6) {
//           Serial.print("Passcode nhập: ");
//           Serial.println(passcode);

//           // Kiểm tra passcode hợp lệ
//           if (kiemtrapasscode(passcode)) {
//             Serial.println("Passcode đúng!");
//             bip(1, 5000, 50, 100);  // Passcode đúng: bip 2 lần
//           } else {
//             Serial.println("Passcode sai!");
//             bip(4, 5000, 50, 100);  // Passcode sai: bip 4 lần
//           }

//           delay(500);
//           resetPasscode();
//         }
//       }
//     }
//   }

//   lastTouched = currentTouched;
// }
String KEY::dockey() {
  uint16_t currentTouched = mpr121.touched();
  for (uint8_t i = 0; i < 12; i++) {
    if ((currentTouched & (1 << i)) && !(lastTouched & (1 << i))) {
      bipkey(5000, 100, 100);
      char key = keyMap[i];
      // Reset passcode nếu nhấn phím '#'
      if (key == '#') {
        resetPasscode();
      } else if (passcode.length() < 6) {
        passcode += key;  // Thêm phím vào passcode
        displayKey();     // Hiển thị passcode đã nhập đến thời điểm hiện tại

        // Kiểm tra nếu passcode đạt 6 ký tự
        if (passcode.length() == 6) {
          String fullPasscode = passcode;  // Lưu lại passcode trước khi reset
          delay(1000);
          resetPasscode();               // Reset passcode sau khi hiển thị
          lastTouched = currentTouched;  // Cập nhật trạng thái cảm ứng
          return fullPasscode;           // Trả về passcode đầy đủ 6 ký tự
        }
      }
    }
  }
  lastTouched = currentTouched;
  return "";
}
String KEY::getKeyPress() {
  uint16_t currentTouched = mpr121.touched();
  for (uint8_t i = 0; i < 12; i++) {
    if ((currentTouched & (1 << i)) && !(lastTouched & (1 << i))) {
      bipkey(5000, 100, 100);
      char key = keyMap[i];
      lastTouched = currentTouched;  // Cập nhật trạng thái cảm ứng
      delay(20);
      return String(key);  // Trả về phím vừa nhấn dưới dạng chuỗi
    }
  }
  lastTouched = currentTouched;
  return "";  // Không có phím nào được nhấn
}

String KEY::nhapid() {
  uint16_t currentTouched = mpr121.touched();
  String keyPressed = "";  // Chuỗi để lưu trữ các ký tự người dùng nhập

  for (uint8_t i = 0; i < 12; i++) {
    if ((currentTouched & (1 << i)) && !(lastTouched & (1 << i))) {
      bipkey(5000, 100, 100);
      char key = keyMap[i];
      keyPressed += key;             // Thêm ký tự vào chuỗi khi nhấn phím
      lastTouched = currentTouched;  // Cập nhật trạng thái cảm ứng
      delay(20);
      return keyPressed;  // Trả về chuỗi đã nhập
    }
  }

  lastTouched = currentTouched;
  return "";  // Không có phím nào được nhấn
}
// String KEY::getKeyPress() {
//   uint16_t currentTouched = mpr121.touched();
//   static uint16_t lastReadTouched = 0;

//   // Kiểm tra nếu tín hiệu cảm ứng thay đổi
//   if (currentTouched != lastReadTouched) {
//     for (uint8_t i = 0; i < 12; i++) {
//       if ((currentTouched & (1 << i)) && !(lastReadTouched & (1 << i))) {  // Kiểm tra nhấn phím
//         bipkey(5000, 100, 100);                                            // Phát âm thanh khi nhấn phím
//         char key = keyMap[i];
//         lastReadTouched = currentTouched;  // Cập nhật trạng thái cảm ứng
//         return String(key);                // Trả về ký tự nhấn
//       }
//     }
//   }

//   lastReadTouched = currentTouched;  // Cập nhật trạng thái nếu không có phím nào được nhấn
//   return "";                         // Không có phím nào được nhấn
// }

// String KEY::nhapid() {
//   uint16_t currentTouched = mpr121.touched();
//   static uint16_t lastReadTouched = 0;
//   String keyPressed = "";  // Chuỗi để lưu trữ các ký tự người dùng nhập
//   if (currentTouched != lastReadTouched) {
//     for (uint8_t i = 0; i < 12; i++) {
//       if ((currentTouched & (1 << i)) && !(lastReadTouched & (1 << i))) {
//         bipkey(5000, 100, 100);
//         char key = keyMap[i];
//         keyPressed += key;                 // Thêm ký tự vào chuỗi khi nhấn phím
//         lastReadTouched = currentTouched;  // Cập nhật trạng thái cảm ứng
//         delay(20);
//         return keyPressed;  // Trả về chuỗi đã nhập
//       }
//     }
//   }
//   lastTouched = currentTouched;
//   return "";  // Không có phím nào được nhấn
// }
