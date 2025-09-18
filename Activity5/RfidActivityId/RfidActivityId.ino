
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ----- PIN DEFINITIONS (based on user's wiring) -----
const int SS_PIN  = 5;    // SDA / SS from your wiring
const int RST_PIN = 0;    // RST <-> D0  (mapped to GPIO0 here; change if needed)
const int SCK_PIN = 18;   // SCK
const int MISO_PIN= 19;   // MISO
const int MOSI_PIN= 23;   // MOSI

// ----- OLED settings (128x64 I2C) -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1    // Not used with I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\nESP32 RFID -> OLED example");

  // Initialize SPI with explicit pins (ESP32 allows custom SPI pins)
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init(); // Init MFRC522
  Serial.println("MFRC522 initialized.");

  // Initialize I2C for OLED (default ESP32 SDA=21, SCL=22). If different, pass Wire.begin(SDA,SCL)
  Wire.begin(); 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is common SSD1306 I2C address
    Serial.println("SSD1306 allocation failed");
    while (1) delay(10);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RFID Reader Ready");
  display.display();
  delay(500);
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    // nothing new
    delay(50);
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    // failed to read
    delay(50);
    return;
  }

  // We have a UID
  String uidHex = "";
  unsigned long long uidNum = 0ULL; // accumulate numeric value (works for typical 4-7 byte UIDs)
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // build hex string
    if (mfrc522.uid.uidByte[i] < 0x10) uidHex += "0";
    uidHex += String(mfrc522.uid.uidByte[i], HEX);
    // accumulate numeric value: shift left 8 and add byte
    uidNum = (uidNum << 8) | mfrc522.uid.uidByte[i];
  }
  uidHex.toUpperCase();

  // Print to Serial
  Serial.print("UID (hex): ");
  Serial.println(uidHex);
  Serial.print("UID (dec): ");
  Serial.println(uidNum);

  // Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("RFID Tag Detected");
  display.setTextSize(2);
  display.setCursor(0, 16);
  // if hex fits, show; else show truncated
  if (uidHex.length() <= 16) {
    display.println(uidHex);
  } else {
    display.println(uidHex.substring(0, 12) + "..."); // short display
  }
  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print("Dec: ");
  display.println(uidNum);
  display.display();

  // Halt PICC and stop encryption on PCD
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // small pause to avoid multiple reads of same tag continuously
  delay(800);
}
