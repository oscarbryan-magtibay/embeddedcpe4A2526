#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// RC522 wiring (change if you used different pins)
#define SS_PIN   5
#define RST_PIN  27

MFRC522 rfid(SS_PIN, RST_PIN);

// OLED setup (I2C default SDA=21, SCL=22)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } // optional, waits for serial

  // SPI + RFID init
  SPI.begin();            // SCK = 18, MOSI = 23, MISO = 19
  rfid.PCD_Init();

  // OLED init
  Wire.begin(); // default SDA=21, SCL=22
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  showTapPrompt();
  Serial.println("Ready. Tap a card/tag on the reader...");
}

void loop() {
  // wait for a new card
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // build UID string (HEX, uppercase, spaced)
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    byte v = rfid.uid.uidByte[i];
    if (v < 0x10) uidStr += "0";
    uidStr += String(v, HEX);
    if (i < rfid.uid.size - 1) uidStr += " ";
  }
  uidStr.toUpperCase();

  // Serial output
  Serial.print("Card UID: ");
  Serial.println(uidStr);

  // OLED: UID then greeting
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 6);
  display.println("UID:");
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.println(uidStr);
  display.setTextSize(2);
  display.setCursor(0, 50);
  display.println("Hello Vergss");
  display.display();

  // cleanup & short delay so you can remove tag
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(3000); // show for 3 seconds
  showTapPrompt();
}

// Helper to show initial prompt
void showTapPrompt() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("TAP CARD/TAG");
  display.display();
}