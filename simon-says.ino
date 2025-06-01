#include <Wire.h>

String addressToString(byte address);
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
}

String addressToString(byte address) {
  char buffer[5];
  snprintf(buffer, sizeof(buffer), "0x%02X", address);
  return String(buffer);
}

String getLcdAddress() {
  Serial.println("Scanning...");
  
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.println("I2C device found at address: " + addressToString(address) + "!");
      return addressToString(address);
    } 
    
    if (error == 4)
      Serial.println("Unknown error at address: " + addressToString(address));
  }

  return "";
}