#include "04_vantay.h"
#include "01_LORA.h"
#include "03_KEY.h"

extern LORA lora;
FingerPrint::FingerPrint(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin)
  : mySerial(serial), finger(&serial) {
  mySerial.begin(57600, SERIAL_8N1, rxPin, txPin);  // Initialize serial for the fingerprint sensor
}
////////////////////////////////////////////////////////////////////////////
bool FingerPrint::isFingerprintIDValid(int fingerprintID) {
  // Duyệt qua tất cả các ID có thể lưu (từ 1 đến 127, hoặc số ID tối đa mà cảm biến hỗ trợ)
  for (int id = 1; id <= 127; id++) {
    if (finger.loadModel(id) == FINGERPRINT_OK) {  // Kiểm tra xem mẫu vân tay tại ID có hợp lệ không
      if (id == fingerprintID) {  // Nếu ID trùng khớp
        Serial.print("Fingerprint ID ");
        Serial.print(fingerprintID);
        Serial.println(" is valid.");
        return true;  // Nếu tìm thấy ID và mẫu vân tay hợp lệ
      }
    }
  }

  Serial.println("Fingerprint ID is not valid.");
  return false;  // Nếu không tìm thấy ID trong bộ nhớ
}

/////////////////////////////////////////////////////////////////////////////////////
void FingerPrint::begin(uint32_t baudrate) {
  finger.begin(baudrate);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }
}
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

uint8_t FingerPrint::enrollFingerprint(uint8_t id) {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as ID #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image taken");
    } else if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Imaging error");
    } else {
      Serial.println("Unknown error");
    }
    delay(100);
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return p;
  }

  Serial.println("Remove finger");
  delay(2000);

  p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    Serial.println("Finger removed");
  }

  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image taken");
    }
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return p;
  }

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else {
    Serial.println("Prints did not match");
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored successfully");
  } else {
    Serial.println("Error storing model");
    return p;
  }

  return p;
}

uint8_t FingerPrint::deleteFingerprint(uint8_t id) {
  int p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else {
    Serial.println("Could not delete");
  }
  return p;
}

// int FingerPrint::getFingerprintID() {
//     int p = finger.getImage();
//       if (p == FINGERPRINT_OK) {
//           Serial.println("Image taken");
//           p = finger.image2Tz();
//           if (p == FINGERPRINT_OK) {
//               Serial.println("Image converted");
//               p = finger.fingerSearch();
//               if (p == FINGERPRINT_OK) {
//                   Serial.println("Fingerprint match found!");
//                   Serial.print("ID: "); Serial.println(finger.fingerID);
//                   Serial.print("Confidence: "); Serial.println(finger.confidence);
//               } else if (p == FINGERPRINT_NOTFOUND) {
//                   Serial.println("No match found");
//               } else {
//                   Serial.print("Error searching fingerprint: "); Serial.println(p);
//               }
//           } else {
//               Serial.print("Error converting image: "); Serial.println(p);
//           }
//       } else {
//           Serial.print("Error getting image: "); Serial.println(p);
//       }
//   }
uint8_t FingerPrint::convertImage() {
  int p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      break;
    default:
      Serial.println("Unknown error");
      break;
  }
  return p;
}
uint8_t FingerPrint::searchFingerprint() {
  int p = finger.getImage();
  if (p == FINGERPRINT_OK) {
    Serial.println("Image taken");
  } else {
    Serial.println("Failed to take image");
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return p;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint match found!");
    Serial.print("ID: ");
    Serial.println(finger.fingerID);
    Serial.print("Confidence: ");
    Serial.println(finger.confidence);
  } else {
    Serial.println("No match found");
  }
  return p;
}
uint8_t FingerPrint ::getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted || Hinh anh duoc chuyen doi");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("\nDid not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return finger.fingerID;
}
uint8_t FingerPrint::downloadFingerprintTemplate(uint16_t id) {
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #");
  Serial.println(id);
  uint8_t p = finger.loadModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.print("Template ");
    Serial.print(id);
    Serial.println(" loaded");
  } else {
    Serial.println("Failed to load template");
    return p;
  }

  Serial.print("Attempting to get #");
  Serial.println(id);
  p = finger.getModel();

  if (p == FINGERPRINT_OK) {
    Serial.print("Template ");
    Serial.print(id);
    Serial.println(" transferring:");
  } else {
    Serial.println("Failed to transfer template");
    return p;
  }

  // Một gói dữ liệu chứa 267 bytes
  uint8_t bytesReceived[534];  // 2 gói dữ liệu, mỗi gói 267 bytes
  memset(bytesReceived, 0xff, 534);

  uint32_t starttime = millis();
  int i = 0;

  // Nhận dữ liệu trong tối đa 20 giây
  while (i < 534 && (millis() - starttime) < 20000) {
    if (mySerial.available()) {
      bytesReceived[i++] = mySerial.read();
    }
  }

  // Nếu số byte nhận được không đủ, thông báo lỗi
  if (i < 534) {
    Serial.println("Error: Not enough bytes received");
    return FINGERPRINT_PACKETRECIEVEERR;
  }

  Serial.print(i);
  Serial.println(" bytes read.");
  Serial.println("Decoding packet...");

  uint8_t fingerTemplate[512];  // Lưu template thực tế
  memset(fingerTemplate, 0xff, 512);

  // Lọc và lấy dữ liệu từ gói
  int uindx = 9, index = 0;
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);  // Lấy 256 bytes đầu tiên
  uindx += 256 + 2 + 9;                                        // Bỏ qua 256 bytes dữ liệu, 2 bytes checksum, và 9 bytes header
  index += 256;
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);  // Lấy 256 bytes tiếp theo

  // In dữ liệu template dưới dạng hex
  for (int i = 0; i < 512; ++i) {
    printHex(fingerTemplate[i], 2);
    Serial.print(" ");
  }

  Serial.println("\ndone.");
  return p;
}

void FingerPrint::printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%0%dX", precision);  // Định dạng để in số hex với số chữ số xác định
  sprintf(tmp, format, num);
  Serial.print(tmp);
}
// returns -1 if failed, otherwise returns ID #
int FingerPrint::getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
// Hàm kiểm tra xem có vân tay hay không
bool FingerPrint::isPress() {
  uint8_t p = finger.getImage();  // Lấy ảnh vân tay
  if (p == FINGERPRINT_OK) {
    return true;  // Có vân tay
  } else {
    return false;  // Không có vân tay
  }
}
