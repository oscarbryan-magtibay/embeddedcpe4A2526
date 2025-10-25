// RFID_LCD_Final_SS21_RST4.ino
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

// ---------- Wiring (must match this)
// RC522:
//   VCC  -> 3.3V
//   GND  -> GND
//   SDA/SS -> GPIO21
//   SCK  -> GPIO18
//   MOSI -> GPIO23
//   MISO -> GPIO19
//   RST  -> GPIO4
//
// I2C LCD (your wiring):
//   VCC -> 5V (or 3.3V if you prefer)
//   GND -> GND
//   SDA -> GPIO25
//   SCL -> GPIO26
// I2C address: 0x27 (your scanner found this)

// ---------- Pins & devices ----------
#define RC522_SS_PIN 21
#define RC522_RST_PIN 4
#define LCD_SDA_PIN 25
#define LCD_SCL_PIN 26
#define LCD_ADDR 0x27

MFRC522 mfrc522(RC522_SS_PIN, RC522_RST_PIN);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// ---------- Whitelist (authorized UIDs) ----------
const uint8_t tag1[] = {0xC0, 0xF2, 0x31, 0x5F};  // RFID tag
const uint8_t tag2[] = {0x83, 0xC7, 0xF8, 0x05};  // RFID card

const uint8_t* authorizedList[] = { tag1, tag2 };
const size_t authorizedLens[] = { sizeof(tag1), sizeof(tag2) };
const size_t AUTH_COUNT = 2;


// ---------- Helpers ----------
String uidToString(const uint8_t *uid, size_t uidLen) {
  String s = "";
  for (size_t i = 0; i < uidLen; ++i) {
    if (uid[i] < 0x10) s += "0";
    s += String(uid[i], HEX);
    if (i + 1 < uidLen) s += ":";
  }
  s.toUpperCase();
  return s;
}

bool uidMatches(const uint8_t *uid, size_t uidLen, const uint8_t *candidate, size_t candLen) {
  if (uidLen != candLen) return false;
  for (size_t i = 0; i < uidLen; ++i) if (uid[i] != candidate[i]) return false;
  return true;
}

int authorizedIndex(const uint8_t *uid, size_t uidLen) {
  for (size_t i = 0; i < AUTH_COUNT; ++i) {
    if (uidMatches(uid, uidLen, authorizedList[i], authorizedLens[i])) return (int)i;
  }
  return -1;
}

// ---------- Setup & Loop ----------
void setup() {
  Serial.begin(115200);
  delay(50);

  // Init I2C on chosen pins (explicit)
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);

  // Init LCD
  lcd.begin();    // Frank de Brabander lib
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RFID Starting...");
  lcd.setCursor(0,1);
  lcd.print("Waiting card...");

  // Init SPI and RC522 (VSPI default: SCK=18, MISO=19, MOSI=23)
  SPI.begin();           // use default pins
  mfrc522.PCD_Init();
  delay(100);
  Serial.println("RC522 initialized (SS=21, RST=4).");
}

void loop() {
  // Wait for a new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(40);
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    delay(40);
    return;
  }

  uint8_t *uid = mfrc522.uid.uidByte;
  size_t uidLen = mfrc522.uid.size;
  String uidStr = uidToString(uid, uidLen);

  Serial.print("Tag UID: ");
  Serial.println(uidStr);

  // Show UID on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UID:");
  lcd.setCursor(0,1);
  if (uidStr.length() <= 16) lcd.print(uidStr); else lcd.print(uidStr.substring(0,16));

  // Whitelist check
  int idx = authorizedIndex(uid, uidLen);
  lcd.setCursor(13,0);
  if (idx >= 0) {
    lcd.print("OK");
    Serial.println("Access: GRANTED");
  } else {
    lcd.print("NO");
    Serial.println("Access: DENIED");
  }

  // Cleanup
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1200);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting card...");
}
