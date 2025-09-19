#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RC522 pins
#define SS_PIN 5
#define RST_PIN 4  

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C   

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void showMessage(const char* line1, const char* line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(line1);

  if (strlen(line2) > 0) {
    display.setCursor(0, 20);
    display.println(line2);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);


  SPI.begin(18, 19, 23, SS_PIN); 
  mfrc522.PCD_Init();


  Wire.begin(21, 22);  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }


  showMessage("RFID + OLED Ready");
  delay(1500);
  showMessage("Tap your card...");
}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uidStr += ":";
  }
  uidStr.toUpperCase();


  Serial.print("Card UID detected: ");
  Serial.println(uidStr);


  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Welcome!");
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("UID: ");
  display.println(uidStr);
  display.display();

  delay(2000);

  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


  showMessage("Tap your card...");
}
