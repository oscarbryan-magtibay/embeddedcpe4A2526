#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN 5
#define RST_PIN 27
#define BUZZER_PIN 26  

MFRC522 rfid(SS_PIN, RST_PIN);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void centerText(const char* text, int y, int textSize) {
  int16_t x1, y1;
  uint16_t w, h;

  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int x = (SCREEN_WIDTH - w) / 2;

  display.setCursor(x, y);
  display.println(text);
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(true);
  }

  display.clearDisplay();
  centerText("Close", 20, 3);
  display.display();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    display.clearDisplay();
    centerText("Close", 20, 3);
    display.display();
    delay(500);
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  display.clearDisplay();
  centerText("Open", 20, 3);
  display.display();

  digitalWrite(BUZZER_PIN, HIGH);  
  delay(300);                      
  digitalWrite(BUZZER_PIN, LOW);   

  delay(1000);

  display.clearDisplay();
  centerText("Welcome", 0, 2);
  centerText("student:", 20, 2);
  centerText("2321087", 40, 2);
  display.display();

  delay(2000);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
