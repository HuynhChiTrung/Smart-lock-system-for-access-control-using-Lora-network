#ifndef FingerPrint_h
#define FingerPrint_h

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>

class FingerPrint {
public:
    FingerPrint(HardwareSerial &serial, uint8_t rxPin, uint8_t txPin);
    void begin(uint32_t baudrate = 57600);
    uint8_t enrollFingerprint(uint8_t id);
    uint8_t deleteFingerprint(uint8_t id);
    // int getFingerprintID();
    uint8_t readnumber();
    uint8_t searchFingerprint();
    uint8_t convertImage();
    uint8_t downloadFingerprintTemplate(uint16_t id);
    uint8_t getFingerprintID();
    void printHex(int num, int precision);
    int getFingerprintIDez();
    bool isPress();
    bool isFingerprintIDValid(int fingerprintID); 
    
private:
    HardwareSerial &mySerial;
    Adafruit_Fingerprint finger;
};

#endif
