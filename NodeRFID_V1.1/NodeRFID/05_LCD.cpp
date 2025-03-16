#include "05_LCD.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Dữ liệu từng đoạn số lớn
uint8_t segments[9][8] = {
  { 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },  // F
  { 0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00 },  // A
  { 0x1c, 0x1e, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },  // B
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f },  // D
  { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x07 },  // E
  { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x1c },  // C
  { 0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x1f },  // G + D
  { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },  // Full Block
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }   // Full Block
};


// Tạo lại ký tự tùy chỉnh với dữ liệu trống để "xóa" nó
//byte empty[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Ký tự trống (tất cả các pixel đều tắt)


// Bảng tra cứu các đoạn để hiển thị số
const uint8_t numbers[19][6] = {
  { 0, 1, 2, 4, 3, 5 },    // Số 0
  { 1, 2, 32, 3, 7, 3 },   // Số 1
  { 6, 6, 2, 4, 3, 3 },    // Số 2
  { 6, 6, 2, 3, 3, 5 },    // Số 3
  { 7, 3, 7, 32, 32, 7 },  // Số 4
  { 7, 6, 6, 3, 3, 5 },    // Số 5
  { 0, 6, 6, 4, 3, 5 },    // Số 6
  { 1, 1, 7, 32, 32, 7 },  // Số 7
  { 0, 6, 2, 4, 3, 5 },    // Số 8
  { 0, 6, 2, 3, 3, 5 },    // Số 9
  { 0, 6, 2, 7, 32, 7 },   // Số A
  { 7, 32, 32, 7, 6, 5 },  // Số B
  { 0, 1, 1, 4, 3, 3 },    // Số C
  { 32, 32, 7, 4, 6, 7 },  // Số D
  { 0, 6, 6, 4, 3, 3 },    // Số E
  { 7, 6, 6, 7, 32, 32 },  // Số F
  { 7, 32, 7, 7, 1, 7 },   // Số H
  { 7, 32, 32, 4, 3, 3 },   // Số L
  { 7, 6, 2, 7, 32, 4 }   // Số R
};
void LCD::khoitao() {
  Wire.begin(33, 32);
  lcd.begin(16, 2);
  lcd.backlight();
  // // Tạo 8 ký tự tùy chỉnh
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, segments[i]);
  }
  // lcd.setCursor(0, 0);
  // lcd.print("---IOT VISION---");
  // lcd.setCursor(0, 1);
  // lcd.print("_Chao Qui Khach_");
  LCD::displayWaitFinger();
}

void LCD::displayshowADD() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" *.OK    #.Exit ");
}
void LCD::displayshowMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Add   2.Delete");
  lcd.setCursor(0, 1);
  lcd.print("3.Search  4.Exit");
}
void LCD::displayWaitFinger() {
  lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("---IOT VISION---");
  // lcd.setCursor(0, 1);
  // lcd.print("_Chao Qui Khach_");
  displayNumber(16, 0, 0);
  displayNumber(14, 3, 0);
  displayNumber(17, 6, 0);
  displayNumber(17, 9, 0);
  displayNumber(0, 12, 0);
}
void LCD::displayClear() {
  lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("---IOT VISION---");
  // lcd.setCursor(0, 1);
  // lcd.print("_Chao Qui Khach_");
  displayNumber(12, 0, 0);
  displayNumber(17, 3, 0);
  displayNumber(14, 6, 0);
  displayNumber(10, 9, 0);
  displayNumber(18, 12, 0);
}
void LCD::displayInvalidFinger() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("VAN TAY");
  lcd.setCursor(0, 1);
  lcd.print("KHONG TIM THAY!!");
}

void LCD::displayFingerOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("XIN MOI BAN VAO");
  lcd.setCursor(1, 1);
  lcd.print("CUA DA DUOC MO");
}
void LCD::displayWaitRFID() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   RFID  SCAN   ");
  lcd.setCursor(0, 1);
  lcd.print("THE KHONG HOP LE");
}
void LCD::displayOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("XIN MOI BAN VAO");
  lcd.setCursor(1, 1);
  lcd.print("CUA DA DUOC MO");
}
void LCD::displayNumber(int num, int col, int row) {
  uint8_t i;
  lcd.setCursor(col, row);  // Đặt con trỏ tại vị trí x, y ban đầu

  // Lặp qua các đoạn số lớn và hiển thị chúng
  for (i = 0; i < 6; i++) {
    if (i == 3) {                   // Sau 3 đoạn, chuyển sang dòng tiếp theo
      lcd.setCursor(col, row + 1);  // Chuyển đến vị trí tiếp theo trên dòng thứ hai
    }
    uint8_t segmentIndex = numbers[num][i];  // Lấy chỉ mục của đoạn số
    if (segmentIndex < 9) {                  // Nếu là ký tự tùy chỉnh (trong `segments`)
      lcd.write(segmentIndex);               // Hiển thị đoạn tương ứng từ CGRAM
    } else {                                 // Nếu là mã ASCII hoặc ngoài phạm vi
      lcd.write(segmentIndex);               // Hiển thị trực tiếp ký tự ASCII
    }
  }
}
