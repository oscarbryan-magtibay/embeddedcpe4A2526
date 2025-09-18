#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// RFID pins
#define SS_PIN  5
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define a struct for users
struct User {
  byte uid[4];
  const char* name;
};

// List of valid users
User users[] = {
  {{0x13, 0x97, 0x1C, 0x2A}, "Denise"},
  {{0x63, 0x3D, 0x75, 0x14}, "Denise"}
};
int userCount = sizeof(users) / sizeof(users[0]);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("Scan your card");
  display.display();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID size: ");
  Serial.println(rfid.uid.size);

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  const char* userName = checkUser(rfid.uid.uidByte, rfid.uid.size);

  if (userName != NULL) {
    String message = "Hi " + String(userName);
    showMessage("Access Granted", message);
    Serial.print("Access Granted to ");
    Serial.println(userName);
  } else {
    showMessage("Access", "Denied");
    Serial.println("Access Denied");
  }

  delay(2000);
  showMessage("Scan", "your card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

const char* checkUser(byte *uid, byte uidSize) {
  for (int u = 0; u < userCount; u++) {
    bool match = true;
    for (byte i = 0; i < uidSize; i++) {
      if (uid[i] != users[u].uid[i]) {
        match = false;
        break;
      }
    }
    if (match) return users[u].name;
  }
  return NULL;
}

void showMessage(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 15);
  display.println(line1);
  display.setCursor(10, 35);
  display.println(line2);
  display.display();
}
