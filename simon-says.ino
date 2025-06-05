#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C* lcd{ nullptr };

constexpr int actionButton = 13;
constexpr int moveButton = 12;
constexpr int Button1 = 5;
constexpr int Button2 = 6;
constexpr int Button3 = 7;
constexpr int Bowser = 9;


constexpr byte Check[8] = {
0b00000,
0b00001,
0b00011,
0b10110,
0b11100,
0b01000,
0b00000,
0b00000
};

String addressToString(byte address);
byte charToByte(char c);
byte stringToByte(const String& str);
String getLcdAddress();
int getPressedMenuButton();

void lcdPrint(const char* message);

void setPins(int first, int last, int mode);
void ejecutar1();
void ejecutar2();
void ejecutar3();

void printMenu(int index);
void printDifficulty(int difficulty);
int chooseDifficulty();

bool executeAction(int index);

void setIdleMessage();
void printWelcome();
void printGoodbye();

void play(int difficulty);

namespace Sound {
  
  void welcome() { 
    tone(Bowser, 150, 150);
    delay(150);
    tone(Bowser, 250, 150);
  }

  void exiting() { 
    tone(Bowser, 250, 150); 
    delay(150);
    tone(Bowser, 150, 150);
  }

  void game() { 
    tone(Bowser, 150, 150);
    delay(150);
    tone(Bowser, 250, 150);
    delay(150);
    tone(Bowser, 350, 150);
    delay(150);
    tone(Bowser, 450, 150);
  }

  void lost() {
    tone(Bowser, 450, 150);
    delay(150);
    tone(Bowser, 350, 150);
    delay(150);
    tone(Bowser, 250, 150);
    delay(150);
    tone(Bowser, 150, 150);
  }

  void chooseAction() { tone(Bowser, 250, 100); }
  void moveMenu() { tone(Bowser, 200, 150); }
  void ledOn() { tone(Bowser, 350, 150); }
  void ledOff() { tone(Bowser, 250, 150); }
}

void setPins(int first, int last, int mode)
{
	for (int led = first; led <= last; ++led)
		pinMode(led, mode);
}

void ejecutar1()
{
  	tone(Bowser, 150, 250);
  	digitalWrite(2, HIGH);
  	digitalWrite(3, LOW);
  	digitalWrite(4, LOW);
  	delay(250);
    digitalWrite(2, LOW);
}

void ejecutar2()
{
  	tone(Bowser, 250, 250);
  	digitalWrite(2, LOW);
  	digitalWrite(3, HIGH);
  	digitalWrite(4, LOW);
 	 delay(250);
    digitalWrite(3, LOW);
}

void ejecutar3()
{
  	tone(Bowser, 350, 250);
  	digitalWrite(2, LOW);
  	digitalWrite(3, LOW);
  	digitalWrite(4, HIGH);
 	 delay(250);
    digitalWrite(4, LOW);
}

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

  lcd->createChar(0, Check);

  pinMode(actionButton, INPUT);
  pinMode(moveButton, INPUT);
  setPins(Button1, Button3, INPUT);
  pinMode(Bowser, OUTPUT);
  setPins(2, 4, OUTPUT);

  setIdleMessage();
}

void loop() {
  static int index{ 0 };
  static bool is_on{ false };

  if (!is_on)
    if (getPressedMenuButton() == actionButton) {
      is_on = true;
      printWelcome();
      delay(1000);
      Sound::welcome();
      printMenu(index);
    } else {
      lcd->scrollDisplayRight();
      delay(200);
      return;
    };

  int pressed{ getPressedMenuButton() };
  if (!pressed)
    return;

  switch(pressed) {
    case actionButton:
      Sound::chooseAction();
      delay(300);  
      is_on = executeAction(index);
      if (!is_on) {
        index = 0;
        printGoodbye();
        delay(1000);
        Sound::exiting();
        setIdleMessage();
        return;
      }
      printMenu(index);
      return;
      break;
    case moveButton:
      ++index;
      index %= 3;
      Sound::moveMenu();
      delay(250);
      printMenu(index);
      break;
    default:
      break;
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
    lcd->scrollDisplayRight();
    delay(delay_time);
  }
}

int getPressedMenuButton() {
  if (digitalRead(actionButton))
    return actionButton;

  if (digitalRead(moveButton))
    return moveButton;

  return 0;
}

int getPressedGameButton() {
  if (digitalRead(Button1))
    return Button1;

  if (digitalRead(Button2))
    return Button2;

  if (digitalRead(Button3))
    return Button3;

  return 0;
}

void printMenu(int index) {
  lcd->clear();

  for (int i{ index }; i < index + 2; ++i) {
      lcd->setCursor(0, (index == i) ? 0 : 1);
      lcd->print((i == 0) ? "1. Start" : (i == 1) ? "2. Difficulty" : (i == 2) ? "3. Exit" : "");
      if (index == i) lcd->write(0);
  }
}

void printDifficulty(int difficulty) {
  lcd->clear();
  difficulty = (difficulty <= 0) ? 4 : difficulty;
  for (int i{ difficulty }; i < difficulty + 2; ++i) {
    lcd->setCursor(0, (difficulty == i) ? 0 : 1);
    lcd->print((i == 1) ? "1. Easy" : (i == 2) ? "2. Medium" : (i == 3) ? "3. Hard" : (i == 4) ? "4. Back" : "");
    if (difficulty == i) lcd->write(0);
  }
}

int chooseDifficulty() {
    int difficulty{ 1 };
    printDifficulty(difficulty);
    
    while (true) {
      int pressed{ getPressedMenuButton() };
  
      if (!pressed)
        continue;

      switch(pressed) {
        case actionButton:
          Sound::chooseAction();
          delay(300);
          return difficulty;
          break;
        case moveButton:
          ++difficulty;
          difficulty %= 4;
          printDifficulty(difficulty);
          Sound::moveMenu();
          delay(250);
          break;
      }
    }

    return 0;
}

bool executeAction(int index) {
  static int difficulty{ 1 };

  if (index == 2)
    return false;
  
  switch (index) {
    case 0:
      play(difficulty);
      break;
    case 1:
      int temp = chooseDifficulty();
      if (temp)
        difficulty = temp;
      break;
    default:
      break;     
  }

  return true;
}

void setIdleMessage() {
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("Press start to play!");
}

void printWelcome() {
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("Welcome!");
}

void printGoodbye() {
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("See you!");
}

void playLed(int led, int reactionTime) {
  digitalWrite(led, HIGH);
  Sound::ledOn();
  delay(reactionTime);
  digitalWrite(led, LOW);
  Sound::ledOff();
}

void play(int difficulty) {
  randomSeed(A0);
  lcd->clear();
  lcd->print("playing...");
  Sound::game();
  int count = 0;
  int randomLed = -1;
 
  int reactionTime = (difficulty == 3) ? 200 : (difficulty == 2) ? 250 : 350;
  int times = 0;

  while (true) {
    if (getPressedMenuButton() == actionButton) {
      Sound::exiting();
      return;
    }

    int pressed = getPressedGameButton();

    ++count;

    if (count % 10 == 0) {
      lcd->scrollDisplayRight();
    }

    delay(10);

    if (randomLed == -1) {
      randomLed = random(0, 3);
      playLed(2 + randomLed, reactionTime);
    }

    if (!pressed)
      continue;

    if (pressed - Button1 == randomLed) {
      randomLed = -1;
      Sound::ledOn();
      delay(reactionTime);
      ++times;
    } else {
      lcd->clear();
      Sound::lost();
      displayScrollingText("Game Over", 2);
      lcd->clear();
      lcd->setCursor(0, 0);
      Sound::moveMenu();
      lcd->print("leds were on");
      lcd->setCursor(0, 1);
      lcd->print(String(times) + " times total");
      delay(2000);
      return;
    }
  }
}