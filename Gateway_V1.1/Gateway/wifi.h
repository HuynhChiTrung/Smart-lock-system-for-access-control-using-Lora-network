#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <set>
#include <vector>
#include "webConfig.h"
#define EEPROM_SIZE 1024
#define BOOT_BUTTON_PIN 0

class WiFiManager {
public:
  void begin();
  void handleSave(AsyncWebServerRequest* request);
  void handleScanWiFi(AsyncWebServerRequest* request);
  void checkButton();
  void getDataFromGoogleSheets();
  void loadWiFiCredentials();
  void saveWiFiCredentials(const char* ssid, const char* password);
  void connectToWiFi();
  void createAccessPoint();
  void scheduleDataFetch();
  void sendCredentials(const String& passcode, const String& rfid, const String& fingerprint);
  void checkWiFiStatusAndUpdateLED();
  String sendRFIDAndPasscodeOverLoRa();
private:
  String ssidStr;
  String passwordStr;
  AsyncWebServer server{ 80 };                // Khởi tạo server trên cổng 80
  String lastData;                            // Biến để lưu trữ dữ liệu cũ
  unsigned long lastFetchTime = 0;            // Thời gian lấy dữ liệu lần cuối
  const unsigned long fetchInterval = 20000;  // Khoảng thời gian giữa các lần lấy dữ liệu (60 giây)
  std::set<int> receivedIDs;   
  int targetID = -1;                 // Khai báo thuộc tính ở đây
};

#endif
