#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- RFID Pins ----------------
#define RST_PIN  17   // Reset pin
#define SS_PIN   5    // SDA/SS pin

// ---------------- OLED Config ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1   // No reset pin on most modules
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- RFID Setup ----------------
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  // Init SPI bus for RC522
  SPI.begin(18, 19, 23); // SCK=18, MISO=19, MOSI=23
  mfrc522.PCD_Init();
  Serial.println("Place your RFID card near the reader...");

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 OLED not found");
    for (;;);
  }

  // Show ready message at startup
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Ready for RFID...");
  display.display();
}

void loop() {
  // If no new card, show "Ready for RFID..."
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("Ready for RFID...");
    display.display();
    delay(200); // avoid flicker
    return;
  }

  // Read UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uid += ":";
  }
  uid.toUpperCase();

  // Print UID to Serial Monitor
  Serial.print("Card UID: ");
  Serial.println(uid);

  // Show UID on OLED
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.println("Card Detected!");
  display.setCursor(0, 30);
  display.setTextSize(2);
  display.println(uid);
  display.display();

  // Halt card to prevent continuous re-reading
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000); // keep UID visible for a second
}