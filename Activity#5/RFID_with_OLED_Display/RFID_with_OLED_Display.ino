#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// RFID setup
#define SS_PIN  5
#define RST_PIN 27
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // Serial Monitor
  Serial.begin(115200);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED not found"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID Reader Init...");
  display.display();
  delay(1000);

  // RFID init
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader ready!");
}

void loop() {
  // Check kung may bagong RFID card
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Waiting for RFID...");
    display.display();
    delay(500);
    return;
  }

  // Kung may nadetect na card
  Serial.print("UID tag : ");
  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();

  // OLED display UID
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RFID Detected!");
  display.setCursor(0, 16);
  display.println("UID:");
  display.setCursor(0, 32);
  display.println(content);
  display.display();

  delay(2000);
}