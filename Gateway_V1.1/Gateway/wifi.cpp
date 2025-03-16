#include "wifi.h"

void WiFiManager::begin() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  loadWiFiCredentials();
  connectToWiFi();

  // Khởi tạo server và khai báo route cho trang cấu hình WiFi
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", INDEX_HTML);  // Chứa mã HTML cho cấu hình WiFi
  });

  // Sử dụng lambda để truyền con trỏ this vào hàm callback
  server.on("/save", HTTP_POST, [this](AsyncWebServerRequest* request) {
    this->handleSave(request);
  });

  server.begin();
}

void WiFiManager::loadWiFiCredentials() {
  EEPROM.begin(EEPROM_SIZE);
  char ssidBuf[32], passBuf[32];
  for (int i = 0; i < 32; ++i) {
    ssidBuf[i] = EEPROM.read(960 + i);
    passBuf[i] = EEPROM.read(992 + i);
  }
  ssidStr = String(ssidBuf);
  passwordStr = String(passBuf);
}

void WiFiManager::saveWiFiCredentials(const char* ssid, const char* password) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 960; i < 1024; ++i) EEPROM.write(i, 0);
  for (int i = 0; i < 32; ++i) {
    EEPROM.write(960 + i, ssid[i]);
    EEPROM.write(992 + i, password[i]);
  }
  EEPROM.commit();
  WiFi.disconnect(true);
  delay(100);
}

void WiFiManager::connectToWiFi() {
  loadWiFiCredentials();
  WiFi.disconnect(true);
  delay(200);
  if (ssidStr.length() > 0 && passwordStr.length() > 0) {
    WiFi.begin(ssidStr.c_str(), passwordStr.c_str());
    Serial.print("Đang kết nối với WiFi: ");
    Serial.println(ssidStr);

    int timeout = 10;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
      delay(300);
      Serial.print(".");
      timeout--;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nKết nối thành công!");
      Serial.print("Địa chỉ IP: ");
      Serial.println(WiFi.localIP());
      WiFi.mode(WIFI_STA);
      delay(500);
      //getDataFromGoogleSheets();
      //sendPasscode("12345678");
    } else {
      Serial.println("\nKết nối thất bại.");
      createAccessPoint();
    }
  } else {
    createAccessPoint();
  }
}

void WiFiManager::createAccessPoint() {
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("ESP32_Config", "12345678");
  Serial.println("Đã tạo Access Point.");
}

void WiFiManager::handleSave(AsyncWebServerRequest* request) {
  String newSSID = request->getParam("ssid", true)->value();
  String newPassword = request->getParam("password", true)->value();
  saveWiFiCredentials(newSSID.c_str(), newPassword.c_str());
  request->send(302, "text/plain", "Thông tin WiFi đã được lưu.");
  delay(1000);
  connectToWiFi();
}

struct Student {
  String id;
  String name;
  String gender;
  String dob;
  String email;
  String phone;
  String rfid;
  String passcode;
  String fingerprint;
};

// Thêm một biến để lưu trữ danh sách ID đã nhận
std::set<int> receivedIDs;
std::vector<int> targetIDs;
// Khai báo danh sách lưu trữ sinh viên
std::vector<Student> students;
const char* scriptURL = "https://script.google.com/macros/s/AKfycbzbxobIgD6OxubmSdzJjUph_sD_7G1M4sQWSow0NJn32rJEWfVGQq89MqmXE-I-F6UtCw/exec";

void WiFiManager::sendCredentials(const String& passcode, const String& rfid, const String& fingerprint) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(scriptURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Tạo dữ liệu POST với tất cả thông số
    String postData = "";
    if (passcode.length() > 0) {
      postData += "passcode=" + passcode;
    }
    if (rfid.length() > 0) {
      if (postData.length() > 0) postData += "&";  // Thêm dấu "&" nếu đã có dữ liệu trước đó
      postData += "rfid=" + rfid;
    }
    if (fingerprint.length() > 0) {
      if (postData.length() > 0) postData += "&";  // Thêm dấu "&" nếu đã có dữ liệu trước đó
      postData += "fingerprint=" + fingerprint;
    }

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode == 302) {
      String redirectUrl = http.getLocation();  // Lấy URL chuyển hướng
      Serial.println("Đang chuyển hướng đến: " + redirectUrl);
      http.end();                     // Kết thúc phiên làm việc cũ
      http.begin(redirectUrl);        // Gửi lại yêu cầu đến URL mới
      httpResponseCode = http.GET();  // Gọi lại yêu cầu

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Dữ liệu nhận được từ Google Sheets: " + response);

        // Phân tích dữ liệu JSON
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
          Serial.print("Lỗi phân tích JSON: ");
          Serial.println(error.f_str());
        } else {
          const char* message = doc["message"];  // Lấy thông điệp từ JSON
          Serial.println("Thông điệp từ server: " + String(message));
        }
      } else {
        Serial.println("Lỗi khi gửi yêu cầu GET: " + String(httpResponseCode));
      }
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void WiFiManager::getDataFromGoogleSheets() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Thêm tham số action=getDataForESP vào URL
    String urlWithParams = scriptURL;          // URL gốc
    urlWithParams += "?action=getDataForESP";  // Thêm tham số action

    http.begin(urlWithParams);  // Gửi yêu cầu đến Google Apps Script
    int httpResponseCode = http.GET();
    if (httpResponseCode == 302) {
      String redirectUrl = http.getLocation();  // Lấy URL chuyển hướng
      Serial.println("Đang chuyển hướng đến: " + redirectUrl);
      http.end();                     // Kết thúc phiên làm việc cũ
      http.begin(redirectUrl);        // Gửi lại yêu cầu đến URL mới
      httpResponseCode = http.GET();  // Gọi lại yêu cầu

      if (httpResponseCode == 200) {
        String response = http.getString();
        Serial.println("Dữ liệu nhận được từ Google Sheets (sau khi chuyển hướng): " + response);
        // Phân tích dữ liệu JSON như trước
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
          students.clear();
          // Lặp qua từng phần tử trong dữ liệu nhận được
          for (JsonObject row : doc.as<JsonArray>()) {
            // Chỉ đọc dữ liệu từ ID 1 trở đi
            int id = row["col1"].as<int>();
            if (id >= 1) {
              Student student;
              student.id = id;
              student.name = row["col2"].as<String>();
              student.gender = row["col3"].as<String>();
              student.dob = row["col4"].as<String>();
              student.email = row["col5"].as<String>();
              student.phone = row["col6"].as<String>();
              student.rfid = row["col7"].as<String>();
              student.passcode = row["col8"].as<String>();
              student.fingerprint = row["col9"].as<String>();

              // Thêm vào danh sách sinh viên
              students.push_back(student);

              Serial.print("ID: ");
              Serial.println(id);
              Serial.print("Họ và Tên: ");
              Serial.println(row["col2"].as<String>());
              Serial.print("Giới tính: ");
              Serial.println(row["col3"].as<String>());
              Serial.print("Ngày sinh: ");
              Serial.println(row["col4"].as<String>());
              Serial.print("Địa chỉ mail: ");
              Serial.println(row["col5"].as<String>());
              Serial.print("Số điện thoại: ");
              Serial.println(row["col6"].as<String>());
              Serial.print("RFID: ");
              Serial.println(row["col7"].as<String>());
              Serial.print("Passcode: ");
              Serial.println(row["col8"].as<String>());
              Serial.print("Vân tay: ");
              Serial.println(row["col9"].as<String>());
            }
          }
        } else {
          Serial.println("Lỗi phân tích dữ liệu JSON từ URL chuyển hướng: " + String(error.c_str()));
        }
      } else {
        Serial.println("Lỗi gửi yêu cầu sau khi chuyển hướng: " + String(httpResponseCode));
      }
    } else {
      Serial.println("Lỗi gửi yêu cầu: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi chưa được kết nối.");
  }
}

void WiFiManager::scheduleDataFetch() {
  // Kiểm tra thời gian để lấy dữ liệu
  if (millis() - lastFetchTime >= fetchInterval) {
    getDataFromGoogleSheets();
    lastFetchTime = millis();
  }
}

void WiFiManager::checkButton() {
  static unsigned long timeButtonPressed = 0;
  static bool isButtonHeld = false;
  if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
    if (!isButtonHeld) {
      timeButtonPressed = millis();
      isButtonHeld = true;
    } else if (millis() - timeButtonPressed >= 3000) {
      createAccessPoint();
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

void WiFiManager::checkWiFiStatusAndUpdateLED() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(13, LOW);  // Bật LED khi có kết nối WiFi
  } else {
    digitalWrite(13, HIGH);  // Tắt LED khi không có kết nối WiFi
  }
}


String WiFiManager::sendRFIDAndPasscodeOverLoRa() {
  String dataToSend = "";

  // Lặp qua danh sách sinh viên
  for (size_t i = 0; i < students.size(); ++i) {
    Student student = students[i];

    // Kiểm tra xem sinh viên có RFID và Passcode không
    if (!student.rfid.isEmpty() && !student.passcode.isEmpty()) {
      if (i > 0) {
        dataToSend += "|";  // Thêm dấu phân cách giữa các dữ liệu nếu không phải phần tử đầu tiên
      }
      // Định dạng RFID:passcode và thêm vào dữ liệu gửi đi
      dataToSend += student.rfid + ":" + student.passcode;
    }
  }

  return dataToSend;
}
