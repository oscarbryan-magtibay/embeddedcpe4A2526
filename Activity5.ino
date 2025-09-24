#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- Pin Definitions ----------------
constexpr uint8_t RST_PIN = 17;   // Reset pin for RC522
constexpr uint8_t SS_PIN  = 5;    // SDA/SS pin for RC522

// ---------------- OLED Settings ----------------
constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET    = -1;   // No reset pin on most OLED modules
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- RFID Setup ----------------
MFRC522 rfid(SS_PIN, RST_PIN);

// ---------------- Helper Functions ----------------
void showMessage(const String &msg, int textSize = 1, int y = 10) {
  oled.clearDisplay();
  oled.setTextSize(textSize);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, y);
  oled.println(msg);
  oled.display();
}

String getCardUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += ":";
  }
  uid.toUpperCase();
  return uid;
}

// ---------------- Main Setup ----------------
void setup() {
  Serial.begin(115200);

  // Init SPI bus for RC522
  SPI.begin(18, 19, 23); // SCK=18, MISO=19, MOSI=23
  rfid.PCD_Init();
  Serial.println("Place your RFID card near the reader...");

  // Init OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 OLED not found");
    while (true); // Stop if no OLED detected
  }

  // Initial message
  showMessage("Ready for RFID...");
}

// ---------------- Main Loop ----------------
void loop() {
  // If no new card detected → show standby message
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    showMessage("Ready for RFID...");
    delay(200);
    return;
  }

  // Read and display UID
  String uid = getCardUID();
  Serial.print("Card UID: ");
  Serial.println(uid);

  oled.clearDisplay();
  oled.setCursor(0, 10);
  oled.setTextSize(1);
  oled.println("Card Detected!");
  oled.setCursor(0, 30);
  oled.setTextSize(2);
  oled.println(uid);
  oled.display();

  // Stop reading continuously
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000); // Keep UID visible
}
