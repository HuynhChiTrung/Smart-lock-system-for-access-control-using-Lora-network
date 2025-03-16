#include "00_RFID.h"
#include "01_LORA.h"
#include "02_RELAY.h"
#include "03_KEY.h"
#include "04_vantay.h"
#include "05_LCD.h"
#include <EEPROM.h>
#include "chuongtrinhchinh.h"

#define EEPROM_SIZE 2048
#define RFID_START_ADDR 0     // Địa chỉ bắt đầu lưu RFID
#define PASS_START_ADDR 1024  // Địa chỉ bắt đầu lưu passcode
#define RFID_LENGTH 11        // Độ dài RFID
#define PASS_LENGTH 6         // Độ dài passcode

RFID rfid;
LORA lora;
RELAY relay;
KEY key;
LCD display;
FingerPrint myFingerPrint(Serial2, 15, 5);

// Mảng lưu RFID và passcode hợp lệ
String validUIDs[100];
String validPasscodes[100];
int validUIDCount = 0;

const int maxRetries = 1;  // Số lần gửi lại tối đa

unsigned long sendStartTime = 0;
const unsigned long sendTimeout = 4000;  // Thời gian chờ phản hồi

bool isInMenu = false;
bool isFingerprintChecking = false;     // Cờ để kiểm tra vân tay độc lập
unsigned long lastFingerCheckTime = 0;  // Thời gian kiểm tra vân tay
unsigned long fingerCheckInterval = 1;  // Khoảng thời gian giữa các lần kiểm tra vân tay

void khoitao() {
  Serial.begin(115200);
  myFingerPrint.begin(57600);
  rfid.khoitao();
  lora.khoitao();
  relay.khoitao();
  key.khoitao();
  display.khoitao();
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  pinMode(0, INPUT_PULLUP);
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Không thể khởi tạo EEPROM");
    delay(1000);
    ESP.restart();
  }

  ///////////////////////////////////////////////////////////////
  //xoadulieuEEPROM();  // Xóa dữ liệu EEPROM nếu cần       /////
  ///////////////////////////////////////////////////////////////

  readValidDataFromEEPROM();  // Đọc UID và passcode từ EEPROM
  // pinMode(buzzer, OUTPUT);
  // digitalWrite(buzzer, LOW);
}
///////////////////////////////////////hàm xử lí EEPROM///////////////////////////////////
void saveValidUIDsToEEPROM() {
  for (int i = 0; i < validUIDCount; i++) {
    char uidStr[RFID_LENGTH + 1];
    validUIDs[i].toCharArray(uidStr, RFID_LENGTH + 1);
    EEPROM.put(RFID_START_ADDR + i * RFID_LENGTH, uidStr);  // Lưu UID vào RFID
  }
  EEPROM.commit();
  Serial.println("Đã lưu validUIDs vào EEPROM");
}

void saveValidPasscodesToEEPROM() {
  for (int i = 0; i < validUIDCount; i++) {
    char passStr[PASS_LENGTH + 1];
    validPasscodes[i].toCharArray(passStr, PASS_LENGTH + 1);
    EEPROM.put(PASS_START_ADDR + i * PASS_LENGTH, passStr);  // Lưu passcode vào vùng riêng
  }
  EEPROM.commit();
  Serial.println("Đã lưu validPasscodes vào EEPROM");
}

void xoadulieuEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);  // Xóa toàn bộ EEPROM
  }
  EEPROM.commit();
  Serial.println("Đã xóa toàn bộ dữ liệu trong EEPROM");
}

void readValidDataFromEEPROM() {
  validUIDCount = 0;
  for (int i = 0; i < 100; i++) {
    char uidRead[RFID_LENGTH + 1];
    char passRead[PASS_LENGTH + 1];

    EEPROM.get(RFID_START_ADDR + i * RFID_LENGTH, uidRead);   // Đọc UID từ vùng RFID
    EEPROM.get(PASS_START_ADDR + i * PASS_LENGTH, passRead);  // Đọc passcode từ vùng pass

    uidRead[RFID_LENGTH] = '\0';  // Thêm ký tự kết thúc chuỗi
    passRead[PASS_LENGTH] = '\0';

    if (String(uidRead) != "" && String(passRead) != "") {
      validUIDs[validUIDCount] = String(uidRead);
      validPasscodes[validUIDCount] = String(passRead);
      validUIDCount++;
    }
  }
  Serial.println("Đã đọc validUIDs và passcodes từ EEPROM");
}

void checkButton() {
  static unsigned long timeButtonPressed = 0;
  static bool isButtonHeld = false;
  if (digitalRead(0) == LOW) {
    if (!isButtonHeld) {
      timeButtonPressed = millis();
      isButtonHeld = true;
    } else if (millis() - timeButtonPressed >= 3000) {
      xoadulieuEEPROM();
      display.displayClear();
      isButtonHeld = false;
    }
  } else {
    if (isButtonHeld) {  // Nếu trạng thái đang giữ mà nhả
      ESP.restart();
      Serial.println("Nút nhả, không giữ đủ 3 giây");
    }
    isButtonHeld = false;
  }
}

// Hàm kiểm tra xem UID có nằm trong mảng không
bool isUIDValid(String uid) {
  for (int i = 0; i < validUIDCount; i++) {
    if (validUIDs[i] == uid) {
      return true;  // UID khớp
    }
  }
  return false;  // UID không hợp lệ
}

bool isPasscodeValid(String passcode) {
  for (int i = 0; i < validUIDCount; i++) {
    if (validPasscodes[i] == passcode) {
      return true;  // UID khớp
    }
  }
  return false;  // UID không hợp lệ
}
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////hàm xử lí vân tay/////////////////////////////
void checkFingerprint() {
  if (!myFingerPrint.isPress()) {
    // Không có vân tay, kết thúc hàm để nhường tài nguyên cho các chức năng khác
    delay(100);
    return;
  }
  // Nếu có vân tay, tiến hành lấy ID và kiểm tra
  int fingerprintID = myFingerPrint.getFingerprintIDez();

  if (fingerprintID == -1) {
    Serial.println("No fingerprint found");  // In ra khi không tìm thấy vân tay
    key.bip(4, 5000, 100, 100);
    display.displayInvalidFinger();  // Hiển thị khi không tìm thấy vân tay
    delay(1000);
  } else {
    // Kiểm tra xem ID vân tay có hợp lệ không
    if (myFingerPrint.isFingerprintIDValid(fingerprintID)) {
      key.bip(1, 5000, 100, 100);  // Phát âm thanh khi vân tay hợp lệ
      display.displayFingerOK();
      relay.dieukhienK1(1);  // Bật relay
      delay(1000);
      relay.dieukhienK1(0);  // Tắt relay sau 1 giây
      delay(1000);
      String data = datasend("", "", fingerprintID);
      lora.guidulieu(data);
    } else {
      Serial.println("Fingerprint ID khong hop le");
      key.bip(4, 5000, 100, 100);  // Phát âm thanh cảnh báo
      display.displayInvalidFinger();
      delay(1000);  // Hiển thị thông báo không hợp lệ
    }
  }
  delay(500);
  display.displayWaitFinger();  // Đặt lại mã passcode sau khi kiểm tra
}

void defaultFingerprintCheck() {
  if (myFingerPrint.getFingerprintIDez() == -1) {
    if (myFingerPrint.isPress()) {  // Kiểm tra nếu cảm biến có vân tay
      display.displayInvalidFinger();
      delay(2000);                  // Hiển thị thông báo trong 2 giây
      display.displayWaitFinger();  // Trở về trạng thái chờ
    }
  } else {
    Serial.println("Bat den !");
    //digitalWrite(buzzer, HIGH);
    relay.dieukhienK1(1);
    display.displayFingerOK();
    delay(3000);
    //digitalWrite(buzzer, LOW);
    relay.dieukhienK1(0);
    display.displayWaitFinger();
  }
  delay(50);
}

void handleMenu() {
  String admin = key.dockey();

  // if (admin == "*1000*") {
  //   isInMenu = true;
  //   display.displayshowMenu();
  // }

  int choice = -1;
  char exitChar = ' ';
  String inputId = "";  // Khai báo biến ngoài switch

  while (choice < 1 || choice > 4) {
    String input = key.getKeyPress();  // Nhận đầu vào từ bàn phím
    if (input.length() > 0) {
      choice = input.toInt();
      if (choice < 1 || choice > 4) {
        Serial.println("Lua chon khong hop le, vui long nhap lai.");
      }
    }
    delay(10);
  }
  int id = -1, del_id = -1;
  switch (choice) {
    case 1:  // Thêm vân tay
      display.displayshowADD();
      exitChar = ' ';
      while (exitChar != '*') {  // Dừng khi nhấn phím '*' để kết thúc nhập
        inputId = "";
        lcd.setCursor(0, 0);
        lcd.print("Nhap ID them: ");              // Hiển thị thông báo trên LCD
                                                  // Đặt lại chuỗi mỗi lần vòng lặp bắt đầu
        while (inputId.length() < 2) {            // Kiểm tra xem người dùng đã nhập đủ ID chưa
          String keyPressed = key.getKeyPress();  // Nhận phím nhấn
          if (keyPressed != "") {
            inputId += keyPressed;  // Thêm ký tự vào ID
            lcd.setCursor(14, 0);
            lcd.print(inputId);  // Hiển thị ID người dùng nhập lên màn hình LCD
            delay(100);          // Đợi 100ms để tránh trùng lặp phím
          }
          if (keyPressed == "#") {
            exitChar = '#';  // Đặt exitChar là '*' để thoát vòng lặp
            display.displayshowMenu();
            break;
          }
        }
        // Kiểm tra điều kiện sau khi thoát hoặc nhập đủ ID
        if (exitChar == '#') {
          break;  // Thoát khỏi vòng lặp thêm vân tay
        }

        // Khi người dùng nhấn '*' để kết thúc nhập ID
        if (inputId.length() > 0) {
          id = inputId.toInt();  // Chuyển chuỗi ID thành số
          if (id >= 0) {
            myFingerPrint.enrollFingerprint(id);  // Thêm vân tay
            Serial.println("ID da duoc them.");
            lcd.setCursor(0, 1);
            lcd.print("ID da duoc them.");
            delay(500);
          } else {
            Serial.println("ID khong hop le.");
            lcd.setCursor(0, 1);
            lcd.print("ID khong hop le.");
            delay(500);
          }
          display.displayshowMenu();  // Quay lại menu
          break;
        }
      }
      break;
    case 2:  // Xóa vân tay
      display.displayshowADD();
      exitChar = ' ';
      while (exitChar != 'X') {  // Dừng khi nhấn phím 'X' để quay lại menu
        inputId = "";            // Đặt lại chuỗi mỗi lần vòng lặp bắt đầu
        lcd.setCursor(0, 0);
        lcd.print("Nhap ID xoa: ");  // Hiển thị thông báo trên LCD

        while (inputId.length() < 2) {            // Kiểm tra xem người dùng đã nhập đủ ID chưa
          String keyPressed = key.getKeyPress();  // Nhận phím nhấn
          if (keyPressed != "") {
            inputId += keyPressed;  // Thêm ký tự vào ID
            lcd.setCursor(14, 0);
            lcd.print(inputId);  // Hiển thị ID người dùng nhập lên màn hình LCD
            delay(100);          // Đợi 100ms để tránh trùng lặp phím
          }
          if (keyPressed == "#") {
            exitChar = '#';             // Đặt exitChar là '#' để thoát vòng lặp
            display.displayshowMenu();  // Quay lại menu
            break;
          }
        }

        // Kiểm tra điều kiện sau khi thoát hoặc nhập đủ ID
        if (exitChar == '#') {
          break;  // Thoát khỏi vòng lặp xóa vân tay
        }
        // Khi người dùng nhấn '*' để kết thúc nhập ID
        if (inputId.length() > 0) {
          id = inputId.toInt();  // Chuyển chuỗi ID thành số
          if (id >= 0) {
            myFingerPrint.deleteFingerprint(id);  // Xóa vân tay
            Serial.println("ID da duoc xoa.");
            lcd.setCursor(0, 1);
            lcd.print("ID da duoc xoa.");
            delay(500);
          } else {
            Serial.println("ID khong hop le.");
            lcd.setCursor(0, 1);
            lcd.print("ID khong hop le.");
            delay(500);
          }
          display.displayshowMenu();  // Quay lại menu
          break;
        }
      }
      break;

    case 3:  // Kiểm tra vân tay
      display.displayshowADD();
      exitChar = ' ';
      while (exitChar != 'X') {  // Dừng khi nhấn phím 'X' để quay lại menu
        lcd.setCursor(0, 0);
        lcd.print("Nhap ID tim: ");  // Hiển thị thông báo trên LCD

        inputId = "";  // Đặt lại chuỗi mỗi lần vòng lặp bắt đầu

        while (inputId.length() < 2) {            // Kiểm tra xem người dùng đã nhập đủ ID chưa
          String keyPressed = key.getKeyPress();  // Nhận phím nhấn
          if (keyPressed != "") {
            inputId += keyPressed;  // Thêm ký tự vào ID
            lcd.setCursor(14, 0);
            lcd.print(inputId);  // Hiển thị ID người dùng nhập lên màn hình LCD
            delay(100);          // Đợi 100ms để tránh trùng lặp phím
          }
          if (keyPressed == "#") {
            exitChar = '#';             // Đặt exitChar là '#' để thoát vòng lặp
            display.displayshowMenu();  // Quay lại menu
            break;
          }
        }

        // Kiểm tra điều kiện sau khi thoát hoặc nhập đủ ID
        if (exitChar == '#') {
          break;  // Thoát khỏi vòng lặp kiểm tra vân tay
        }

        // Khi người dùng nhấn '*' để kết thúc nhập ID
        if (inputId.length() > 0) {
          id = inputId.toInt();              // Chuyển chuỗi ID thành số
          myFingerPrint.getFingerprintID();  // Kiểm tra vân tay

          if (Serial.peek() == 'X') {
            exitChar = Serial.read();  // Kiểm tra nếu nhấn 'X'
            Serial.println("Da quay lai menu.");
            display.displayshowMenu();  // Quay lại menu
            break;
          }
        }
      }
      break;
    case 4:
      Serial.println("Da quay lai menu.");
      isInMenu = false;
      display.displayWaitFinger();
      break;
  }
  choice = -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////hàm nhận dữ liệu lora///////////////////////////
void processDataFromLoRa(String receivedData) {
  if (receivedData == "") return;

  //Serial.println("Bắt đầu phân tích dữ liệu nhận: " + receivedData);
  int startIndex = 0, endIndex = 0;

  while ((endIndex = receivedData.indexOf('|', startIndex)) != -1) {
    String pair = receivedData.substring(startIndex, endIndex);
    processPair(pair);
    startIndex = endIndex + 1;
  }

  // Xử lý cặp cuối cùng nếu còn sót lại
  if (startIndex < receivedData.length()) {
    String lastPair = receivedData.substring(startIndex);
    processPair(lastPair);
  }

  Serial.println("Phân tích dữ liệu hoàn tất.");
}

void processPair(String pair) {
  int delimiterIndex = pair.indexOf(':');
  if (delimiterIndex != -1) {
    String uid = pair.substring(0, delimiterIndex);
    String passcode = pair.substring(delimiterIndex + 1);

    //Serial.println("Phân tích cặp: UID = " + uid + ", Passcode = " + passcode);

    if (!isUIDValid(uid) && !isPasscodeValid(passcode) && validUIDCount < 100) {
      validUIDs[validUIDCount] = uid;
      validPasscodes[validUIDCount] = passcode;
      validUIDCount++;
      saveValidUIDsToEEPROM();
      saveValidPasscodesToEEPROM();
      Serial.println("Nhận UID và passcode mới: " + uid + " - " + passcode +" - " + validUIDCount);
    } else {
      Serial.println("UID hoặc passcode đã tồn tại hoặc vượt quá giới hạn");
    }
  } else {
    Serial.println("Không tìm thấy ký tự ':' trong cặp: " + pair);
  }
}
// void processPair(String pair) {
//   int firstDelimiterIndex = pair.indexOf(':');
//   int secondDelimiterIndex = pair.indexOf(':', firstDelimiterIndex + 1);

//   if (firstDelimiterIndex != -1 && secondDelimiterIndex != -1) {
//     // Tách validUIDCount, uid và passcode
//     String validUIDCountStr = pair.substring(0, firstDelimiterIndex);
//     String uid = pair.substring(firstDelimiterIndex + 1, secondDelimiterIndex);
//     String passcode = pair.substring(secondDelimiterIndex + 1);

//     // Chuyển validUIDCountStr sang kiểu số nguyên
//     int validUIDCount = validUIDCountStr.toInt() - 1;  // Trừ đi 1 để làm chỉ số mảng

//     // Kiểm tra nếu UID và passcode chưa tồn tại và validUIDCount hợp lệ
//     if (validUIDCount >= 0 && validUIDCount < 100 && !isUIDValid(uid) && !isPasscodeValid(passcode)) {
//       validUIDs[validUIDCount] = uid;
//       validPasscodes[validUIDCount] = passcode;
//       saveValidUIDsToEEPROM();
//       saveValidPasscodesToEEPROM();
//       Serial.println("Nhận UID và passcode mới: " + uid + " - " + passcode);
//     } else {
//       Serial.println("UID hoặc passcode đã tồn tại hoặc vượt quá giới hạn");
//     }
//   } else {
//     Serial.println("Không tìm thấy ký tự ':' trong cặp: " + pair);
//   }
// }


//////////////////////////////////////////////////////////////////////////////////////////
// void dieukhienrelay() {
//   //UID hợp lệ, mở relay
//   Serial.println("UID hợp lệ, mở relay");
//   relay.dieukhienK1(1);
//   delay(1000);
//   relay.dieukhienK1(0);
// }
// void checkButton() {
//   static unsigned long timeButtonPressed = 0;
//   static bool isButtonHeld = false;
//   if (digitalRead(0) == LOW) {
//     if (!isButtonHeld) {
//       timeButtonPressed = millis();
//       isButtonHeld = true;
//     } else if (millis() - timeButtonPressed >= 3000) {
//       xoadulieuEEPROM();
//       delay(100);
//       readValidDataFromEEPROM();
//       display.displayClear();
//       delay(1500);
//       display.displayWaitFinger();
//       isButtonHeld = false;
//     }
//   } else {
//     isButtonHeld = false;
//   }
// }
void sendDataWithRetry(const String &uidStr) {
  int attempt = 0;
  while (attempt < maxRetries) {
    // Gửi dữ liệu
    lora.guidulieu(uidStr);
    //Serial.println("Da gui: " + uidStr);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    sendStartTime = millis();

    // Chờ nhận phản hồi
    while (millis() - sendStartTime < sendTimeout) {
      //String receivedData = lora.nhandulieu();
      // if (receivedData.indexOf("ok") != -1) {
      //   //dieukhienrelay();
      //   Serial.println("Nhan duoc phan hoi 'ok' tu node trung gian");
      //   return; // Dừng hàm nếu nhận được phản hồi
      // }
    }

    // Nếu không nhận được phản hồi, thử gửi lại
    Serial.println("Chua nhan duoc 'ok', thu lai...");
    attempt++;
  }

  Serial.println("Khong the nhan phan hoi 'ok' sau " + String(maxRetries) + " lan thu.");
}

void handleRFID() {
  // Gọi hàm docthe() để lấy UID từ thẻ quét được
  String uidStr = rfid.docthe();
  // // Kiểm tra nếu UID không rỗng
  if (uidStr != "") {
    lcd.setCursor(0, 0);
    lcd.print("    THE RFID    ");
    lcd.setCursor(0, 1);
    lcd.print("  DANG QUET..   ");
    delay(500);
    // key.resetPasscode();
    // Kiểm tra xem UID có nằm trong mảng hợp lệ hay không
    if (isUIDValid(uidStr)) {
      key.bip(1, 5000, 100, 100);
      display.displayOK();
      relay.dieukhienK1(1);
      delay(1000);
      relay.dieukhienK1(0);
      digitalWrite(13, 0);
      delay(100);
      digitalWrite(13, 1);
      delay(1000);
      String data = datasend(uidStr, "", -1);
      lora.guidulieu(data);
    } else {
      Serial.println("UID khong hop le");
      key.bip(4, 5000, 100, 100);
      display.displayWaitRFID();
      delay(500);
      // key.resetPasscode();
    }
    delay(500);
    display.displayWaitFinger();
  }
}

void handlePasscode() {
  String fullPasscode = key.dockey();

  if (fullPasscode != "") {
    // Kiểm tra xem UID có nằm trong mảng hợp lệ hay không
    if (isPasscodeValid(fullPasscode)) {
      key.bip(1, 5000, 100, 100);
      display.displayOK();
      relay.dieukhienK1(1);
      delay(1000);
      relay.dieukhienK1(0);
      //lora.guidulieu(ID);
      digitalWrite(13, 0);
      delay(100);
      digitalWrite(13, 1);
      delay(1000);
      String data = datasend("", fullPasscode, -1);
      lora.guidulieu(data);
    } else {
      key.bip(4, 5000, 100, 100);
      Serial.println("UID khong hop le");
      lcd.setCursor(0, 1);
      lcd.print("    PASS SAI    ");
      delay(500);
      // key.resetPasscode();
    }
    delay(500);
    display.displayWaitFinger();
  }
  // //lora.nhandulieu(); // Nhận dữ liệu từ LoRa
  //kiemtravantay();
  //String passcode = key.dockey();  // Nhận passcode từ bàn phím

  if (fullPasscode == "*1000*") {
    isInMenu = true;
    display.displayshowMenu();
  }
}

void showMenu() {
  Serial.println("1: Enroll fingerprint");
  Serial.println("2: Delete fingerprint");
  Serial.println("3: Search for fingerprint");
  Serial.println("Nhap 'X' de quay lai che do kiem tra vân tay.");
  display.displayshowMenu();
}

String datasend(String rfid, String passcode, int fingerprintID) {
  String data = "";  // Khởi tạo chuỗi trống

  // Kiểm tra và thêm giá trị vào chuỗi nếu có
  if (rfid != "") {
    data += "rfid=" + rfid + ";";  // Thêm dữ liệu RFID vào chuỗi
  }
  if (passcode != "") {
    data += "pass=" + passcode + ";";  // Thêm dữ liệu passcode vào chuỗi
  }
  if (fingerprintID != -1) {
    data += "fingerprintID=" + String(fingerprintID) + ";";  // Thêm dữ liệu fingerprintID vào chuỗi
  }
  return data;  // Trả về chuỗi dữ liệu
}

void chuongtrinhchinh() {
  checkButton();
  String receivedData = lora.nhandulieu();
  if (receivedData != "") {
    processDataFromLoRa(receivedData);
  }
  // Kiểm tra RFID
  handleRFID();

  // Kiểm tra passcode
  handlePasscode();

  if (!isInMenu) {
    checkFingerprint();
  } else {
    // Nếu đang ở trong menu, xử lý các lệnh từ bàn phím
    handleMenu();
  }
}


//...................................................chuong trinh test..............................................................................................
// #include "00_RFID.h"
// #include "01_LORA.h"
// #include "02_RELAY.h"
// #include "chuongtrinhchinh.h"
// #include <Arduino.h>
// #include <EEPROM.h>

// RFID rfid;
// LORA lora;
// RELAY relay;

// // Mảng lưu RFID và passcode hợp lệ
// String validUIDs[100];
// String validPasscodes[100];
// int validUIDCount = 0;

// #define EEPROM_SIZE 2048
// #define RFID_START_ADDR 0     // Địa chỉ bắt đầu lưu RFID
// #define PASS_START_ADDR 1024  // Địa chỉ bắt đầu lưu passcode
// #define RFID_LENGTH 11        // Độ dài RFID
// #define PASS_LENGTH 6         // Độ dài passcode

// void saveValidUIDsToEEPROM() {
//   for (int i = 0; i < validUIDCount; i++) {
//     char uidStr[RFID_LENGTH + 1];
//     validUIDs[i].toCharArray(uidStr, RFID_LENGTH + 1);
//     EEPROM.put(RFID_START_ADDR + i * RFID_LENGTH, uidStr);  // Lưu UID vào RFID
//   }
//   EEPROM.commit();
//   Serial.println("Đã lưu validUIDs vào EEPROM");
// }

// void saveValidPasscodesToEEPROM() {
//   for (int i = 0; i < validUIDCount; i++) {
//     char passStr[PASS_LENGTH + 1];
//     validPasscodes[i].toCharArray(passStr, PASS_LENGTH + 1);
//     EEPROM.put(PASS_START_ADDR + i * PASS_LENGTH, passStr);  // Lưu passcode vào vùng riêng
//   }
//   EEPROM.commit();
//   Serial.println("Đã lưu validPasscodes vào EEPROM");
// }

// void readValidDataFromEEPROM() {
//   for (int i = 0; i < 100; i++) {
//     char uidRead[RFID_LENGTH + 1];
//     char passRead[PASS_LENGTH + 1];

//     EEPROM.get(RFID_START_ADDR + i * RFID_LENGTH, uidRead);   // Đọc UID từ vùng RFID
//     EEPROM.get(PASS_START_ADDR + i * PASS_LENGTH, passRead);  // Đọc passcode từ vùng pass

//     uidRead[RFID_LENGTH] = '\0';  // Thêm ký tự kết thúc chuỗi
//     passRead[PASS_LENGTH] = '\0';

//     if (String(uidRead) != "" && String(passRead) != "") {
//       validUIDs[validUIDCount] = String(uidRead);
//       validPasscodes[validUIDCount] = String(passRead);
//       validUIDCount++;
//     }
//   }
//   Serial.println("Đã đọc validUIDs và passcodes từ EEPROM");
// }

// void xoadulieuEEPROM() {
//   for (int i = 0; i < EEPROM_SIZE; i++) {
//     EEPROM.write(i, 0);  // Xóa toàn bộ EEPROM
//   }
//   EEPROM.commit();
//   Serial.println("Đã xóa toàn bộ dữ liệu trong EEPROM");
// }

// void khoitao() {
//   rfid.khoitao();
//   lora.khoitao();
//   relay.khoitao();
//   pinMode(13, OUTPUT);
//   digitalWrite(13, 1);
//   Serial.begin(115200);

//   if (!EEPROM.begin(EEPROM_SIZE)) {
//     Serial.println("Không thể khởi tạo EEPROM");
//     delay(1000);
//     ESP.restart();
//   }

//   ///////////////////////////////////////////////////////////////
//   /// xoadulieuEEPROM();  // Xóa dữ liệu EEPROM nếu cần       /////
//   ///////////////////////////////////////////////////////////////

//   readValidDataFromEEPROM();  // Đọc UID và passcode từ EEPROM
// }

// bool isUIDValid(String uid) {
//   for (int i = 0; i < validUIDCount; i++) {
//     if (validUIDs[i] == uid) {
//       return true;
//     }
//   }
//   return false;
// }

// bool isPasscodeValid(String passcode) {
//   for (int i = 0; i < validUIDCount; i++) {
//     if (validPasscodes[i] == passcode) {
//       return true;
//     }
//   }
//   return false;
// }

// void dieukhienrelay() {
//   Serial.println("UID hoặc passcode hợp lệ, mở relay");
//   relay.dieukhienK1(1);
//   delay(1000);
//   relay.dieukhienK1(0);
// }

// void processDataFromLoRa(String receivedData) {
//   if (receivedData == "") return;

//   //Serial.println("Bắt đầu phân tích dữ liệu nhận: " + receivedData);
//   int startIndex = 0, endIndex = 0;

//   while ((endIndex = receivedData.indexOf('|', startIndex)) != -1) {
//     String pair = receivedData.substring(startIndex, endIndex);
//     processPair(pair);
//     startIndex = endIndex + 1;
//   }

//   // Xử lý cặp cuối cùng nếu còn sót lại
//   if (startIndex < receivedData.length()) {
//     String lastPair = receivedData.substring(startIndex);
//     processPair(lastPair);
//   }

//   Serial.println("Phân tích dữ liệu hoàn tất.");
// }

// void processPair(String pair) {
//   int delimiterIndex = pair.indexOf(':');
//   if (delimiterIndex != -1) {
//     String uid = pair.substring(0, delimiterIndex);
//     String passcode = pair.substring(delimiterIndex + 1);

//     //Serial.println("Phân tích cặp: UID = " + uid + ", Passcode = " + passcode);

//     if (!isUIDValid(uid) && !isPasscodeValid(passcode) && validUIDCount < 100) {
//       validUIDs[validUIDCount] = uid;
//       validPasscodes[validUIDCount] = passcode;
//       validUIDCount++;
//       saveValidUIDsToEEPROM();
//       saveValidPasscodesToEEPROM();
//       Serial.println("Nhận UID và passcode mới: " + uid + " - " + passcode);
//     } else {
//       Serial.println("UID hoặc passcode đã tồn tại hoặc vượt quá giới hạn");
//     }
//   } else {
//     Serial.println("Không tìm thấy ký tự ':' trong cặp: " + pair);
//   }
// }

// void chuongtrinhchinh() {
//   String uidStr = rfid.docthe();

//   if (uidStr != "") {
//     if (isUIDValid(uidStr)) {
//       dieukhienrelay();
//     } else {
//       Serial.println("No ID");
//     }
//   }

//   String receivedData = lora.nhandulieu();
//   if (receivedData != "") {
//     processDataFromLoRa(receivedData);
//   }
// }



//................................................chuong trinh chinh..............................................................................................................................................
// #include "00_RFID.h"
// #include "01_LORA.h"
// #include "02_RELAY.h"
// #include "chuongtrinhchinh.h"
// #include <Arduino.h>
// #include <EEPROM.h>

// RFID rfid;
// LORA lora;
// RELAY relay;

// // Mảng để lưu trữ các UID đã quét
// String uidList[100];     // Giả định mảng chứa tối đa 100 UID
// int uidStoredCount = 0;  // Biến đếm số lượng UID trong mảng

// // Biến thời gian cho việc gửi dữ liệu định kỳ
// unsigned long lastSendTime = 0;
// const unsigned long sendInterval = 1 * 60 * 1000;  // 30 phút (theo milliseconds)

// // Mảng UID hợp lệ
// String validUIDs[100];  // Giả định có thể lưu tối đa 100 UID hợp lệ
// int validUIDCount = 0;  // Biến đếm số lượng UID hợp lệ

// #define EEPROM_SIZE 1024
// #define STRING_LENGTH 11  // Mỗi chuỗi dài 11 ký tự

// void saveValidUIDsToEEPROM() {
//   for (int i = 0; i < validUIDCount; i++) {
//     char str[STRING_LENGTH + 1]; // Tạo mảng có thêm chỗ cho ký tự kết thúc '\0'
//     validUIDs[i].toCharArray(str, STRING_LENGTH + 1); // Chuyển đổi String sang mảng ký tự
//     EEPROM.put(i * STRING_LENGTH, str);  // Ghi chuỗi vào EEPROM
//   }
//   EEPROM.commit(); // Ghi tất cả thay đổi vào bộ nhớ thực
//   Serial.println("Đã lưu validUIDs vào EEPROM");
// }

// void readValidUIDsFromEEPROM() {
//   for (int i = 0; i < 100; i++) {
//     char readValue[STRING_LENGTH + 1]; // Tạo mảng có thêm chỗ cho ký tự kết thúc '\0'
//     EEPROM.get(i * STRING_LENGTH, readValue); // Đọc mảng ký tự từ EEPROM
//     readValue[STRING_LENGTH] = '\0'; // Thêm ký tự kết thúc '\0'

//     // Nếu UID không phải là chuỗi rỗng, thêm vào mảng validUIDs
//     if (String(readValue) != "") {
//       validUIDs[validUIDCount] = String(readValue);
//       validUIDCount++;
//     }
//   }
//   Serial.println("Đã đọc validUIDs từ EEPROM");
// }
// void xoadulieuEEPROM(){
//     // Xóa EEPROM nếu cần
//   for (int i = 0; i < EEPROM_SIZE; i++) {
//     EEPROM.write(i, 0); // Xóa EEPROM
//   }
//   EEPROM.commit(); // Ghi lại thay đổi
// }
// void khoitao() {
//   rfid.khoitao();
//   lora.khoitao();
//   relay.khoitao();
//   pinMode(13, OUTPUT);
//   digitalWrite(13, 1);
//   Serial.begin(9600);
//   if (!EEPROM.begin(EEPROM_SIZE)) {
//     Serial.println("Failed to initialise EEPROM");
//     delay(1000);
//     ESP.restart();  // Khởi động lại nếu không thành công
//   }

// ///////////////////////////////////////////////////////////////
// //  xoadulieuEEPROM();// Xoa du lieu EEPROM                 /////
// ///////////////////////////////////////////////////////////////

//   readValidUIDsFromEEPROM();  // Đọc validUIDs từ EEPROM
// }

// bool isUIDValid(String uid) {
//   for (int i = 0; i < validUIDCount; i++) {
//     if (validUIDs[i] == uid) {
//       return true;  // UID khớp
//     }
//   }
//   return false;  // UID không hợp lệ
// }

// void dieukhienrelay() {
//   Serial.println("UID hợp lệ, mở relay");
//   relay.dieukhienK1(1);
//   delay(1000);
//   relay.dieukhienK1(0);
// }

// void chuongtrinhchinh() {
//   String uidStr = rfid.docthe();

//   if (uidStr != "") {
//     if (isUIDValid(uidStr)) {
//       dieukhienrelay();
//       bool uidExists = false;

//       for (int i = 0; i < uidStoredCount; i++) {
//         if (uidList[i] == uidStr) {
//           uidExists = true;
//           break;
//         }
//       }

//       if (!uidExists && uidStoredCount < 100) {  // Giới hạn tối đa là 100 UID
//         uidList[uidStoredCount] = uidStr;
//         uidStoredCount++;
//         Serial.println("UID stored: " + uidStr);
//       }
//     } else {
//       Serial.println("No ID");
//     }
//   }

//   // Kiểm tra nếu đã đến thời điểm gửi dữ liệu
//   if (millis() - lastSendTime >= sendInterval) {
//     if (uidStoredCount > 0) {
//       Serial.println("Sending data to LoRa...");
//       for (int i = 0; i < uidStoredCount; i++) {
//         lora.guidulieu(uidList[i]);
//         delay(200);
//         digitalWrite(13, 0);
//         delay(200);
//         digitalWrite(13, 1);
//         delay(200);
//       }
//       uidStoredCount = 0;
//       Serial.println("Data sent and list cleared.");
//     }
//     lastSendTime = millis();
//   }

//   // Nhận dữ liệu từ LoRa để điều khiển relay
//   String receivedData = lora.nhandulieu();
//   if (receivedData != "" && !isUIDValid(receivedData)) {
//     if (validUIDCount < 100) {  // Giới hạn số lượng validUIDs
//       validUIDs[validUIDCount] = receivedData;
//       validUIDCount++;
//       saveValidUIDsToEEPROM();
//       digitalWrite(13, 0);
//       delay(200);
//       digitalWrite(13, 1);
//       delay(200);
//       Serial.println("Received valid UID added: " + receivedData);
//     } else {
//       Serial.println("Valid UID array is full.");
//     }
//   }
// }
