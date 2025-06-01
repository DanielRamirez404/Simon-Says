#include <Wire.h>

String addressToString(byte address);
byte charToByte(char c);
byte stringToByte(const String& str);
String getLcdAddress();

void setup() {
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);
}

void loop() {
  static String lcd_address{ getLcdAddress() };

  if (lcd_address == "")
    return;

  Serial.println(stringToByte(lcd_address), HEX);
}

String addressToString(byte address) {
  char buffer[5];
  snprintf(buffer, sizeof(buffer), "0x%02X", address);
  return String(buffer);
}

byte charToByte(char c) {
  return (c >= '0' && c <= '9') ? c - '0' 
    : 10 + c - (c >= 'A' && c <= 'F') ? 'A' : 'a';
}

byte stringToByte(const String& str) {
  int start{ 2 };
  int len{ str.length() };
  
  byte result{ 0 };
  
  for (int i{ start }; i < start + 2 && i < len; i++)
    result = result * 16 + charToByte(str[i]);
  
  return result;
}

String getLcdAddress() {
  Serial.println("Scanning...");
  
  for (byte address{ 1 }; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error { Wire.endTransmission() };

    if (!error) {
      Serial.println("I2C device found at address: " + addressToString(address) + "!");
      return addressToString(address);
    } 
    
    if (error == 4)
      Serial.println("Unknown error at address: " + addressToString(address));
  }

  return "";
}