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

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Close");
  display.display();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(10, 20);
    display.println("Close");
    display.display();
    delay(500);
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.println("Open Welcome");
  display.display();

  digitalWrite(BUZZER_PIN, HIGH);  
  delay(300);                      
  digitalWrite(BUZZER_PIN, LOW);   

  delay(2000);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
