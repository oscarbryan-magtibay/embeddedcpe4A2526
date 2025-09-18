#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RC522 wiring (unchanged)
#define SS_PIN   5
#define RST_PIN  27
MFRC522 rfid(SS_PIN, RST_PIN);

// OLED setup (I2C default SDA=21, SCL=22)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ✅ Authorized UIDs
String authorizedUID1 = "EE 20 7E 05";
String authorizedUID2 = "4A FC 84 04";

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  // SPI + RFID init
  SPI.begin();            // SCK=18, MOSI=23, MISO=19
  rfid.PCD_Init();

  // OLED init
  Wire.begin(); // SDA=21, SCL=22
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

  // build UID string
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    byte v = rfid.uid.uidByte[i];
    if (v < 0x10) uidStr += "0";
    uidStr += String(v, HEX);
    if (i < rfid.uid.size - 1) uidStr += " ";
  }
  uidStr.toUpperCase();

  Serial.print("Card UID: ");
  Serial.println(uidStr);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);

  if (uidStr == authorizedUID1 || uidStr == authorizedUID2) {
    display.println("ACCESS");
    display.setCursor(0, 45);
    display.println("GRANTED");
    Serial.println("ACCESS GRANTED");
  } else {
    display.println("ACCESS");
    display.setCursor(0, 45);
    display.println("DENIED");
    Serial.println("ACCESS DENIED");
  }
  display.display();

  // cleanup
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(3000);
  showTapPrompt();
}

// Helper to show initial prompt
void showTapPrompt() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("TAP CARD");
  display.display();
}
