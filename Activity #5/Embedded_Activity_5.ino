#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

// OLED Pinouts
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RFID Pinouts
#define SS_PIN 5
#define RST_PIN -1
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22); // SDA=21, SCL=22

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Please scan your");
  display.setCursor(0, 35);
  display.println("access card...");
  display.display();

  // RFID init
  SPI.begin();  
  rfid.PCD_Init();
  Serial.println("RFID ready. Waiting for card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Display welcome message
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("Welcome!");
  display.display();

  delay(2000);

  // Reset message
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Please scan your");
  display.setCursor(0, 35);
  display.println("access card...");
  display.display();

  // Halt PICC
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
