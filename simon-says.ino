#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C* lcd = nullptr;

String addressToString(byte address);
byte charToByte(char c);
byte stringToByte(const String& str);
String getLcdAddress();


void displayScrollingText(const char* text, int seconds);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);

  const String lcd_address{ getLcdAddress() };

  while (lcd_address == "")
    Serial.println("Error: LCD address not found");

  const byte lcd_byteAddress{ stringToByte(lcd_address) };

  lcd = new LiquidCrystal_I2C{lcd_byteAddress, 16 , 2};

  lcd->init();
  lcd->clear();         
  lcd->backlight();
  
  lcd->setCursor(0,0);
  lcd->print("If only you paid attention to me, girl...");

  pinMode(13, INPUT);
  pinMode(8, INPUT);
}

void loop() {
  //displayScrollingText("Just the Two of Us", 30);

  if (digitalRead(8) == HIGH) {
    Serial.println("izquierda");
    delay(500);
  } else if(digitalRead(13) == HIGH) {
    Serial.println("derecha");
    delay(500);
  }
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

void displayScrollingText(const char* text, int seconds) {
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(text);
  
  delay(2500);

  constexpr int delay_time{ 500 };

  int i{ 0 };

  for (int elapsed_time{ 0 }; elapsed_time < seconds * 1000; [&]() { elapsed_time += delay_time; ++i; }() ) {
    lcd->scrollDisplayLeft();
    delay(delay_time);
  }
}