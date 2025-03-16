#ifndef _chuongtrinhchinh_h
#define _chuongtrinhchinh_h

void khoitao(void);
void chuongtrinhchinh(void);
void sendDataTask(void * parameter) ;
void dieukhienrelay(void);
void sendDataWithRetry(const String &uidStr);
void timerISR(void);
void readValidUIDsFromEEPROM();
void saveValidUIDsToEEPROM();
void xoadulieuEEPROM();
void kiemtravantay();
void kiemtravantay(String fullPasscode) ;
void defaultFingerprintCheck();
void handleMenu();
void showMenu();
void displayshowADD();
void displayshowMenu();
void displayWaitFinger();
void displayInvalidFinger();
void displayFingerOK();
void handleOtherTasks();
void checkButton();
void processDataFromLoRa(String receivedData);
void processPair(String pair) ;
void readValidDataFromEEPROM();
void saveValidPasscodesToEEPROM();

String datasend(String rfid, String passcode, int fingerprintID);
#endif