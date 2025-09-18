#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN      5
#define RST_PIN     4
#define BUZZER_PIN 25
#define LED_PIN    26

MFRC522 rfid(SS_PIN, RST_PIN);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct User {
  byte uid[4];
  const char* name;
};

User users[] = {
  {{0xB1, 0x02, 0x71, 0x05}, "Master JP"},
  {{0x06, 0xD5, 0x94, 0x04}, "Master JP"},
  {{0x11, 0x22, 0x33, 0x44}, "Guest 1"},
  {{0xAA, 0xBB, 0xCC, 0xDD}, "Guest 2"}
};
int userCount = sizeof(users) / sizeof(users[0]);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    for (;;);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 20);
  display.println(" RFID Access ");
  display.setCursor(25, 40);
  display.println(" ESP32 Ready ");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID Tag: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  const char* userName = checkUser(rfid.uid.uidByte);

  if (userName != NULL) {
    Serial.print("Access Granted to ");
    Serial.println(userName);
    if (String(userName) == "Master JP") {
      showResult("Hello", "Master JP", true);
    } else {
      showResult("Hello", userName, true);
    }
  } else {
    Serial.println("Access Denied!");
    showResult("ACCESS", "DENIED", false);
  }

  delay(2000);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

const char* checkUser(byte *uid) {
  for (int u = 0; u < userCount; u++) {
    bool match = true;
    for (byte i = 0; i < 4; i++) {
      if (uid[i] != users[u].uid[i]) {
        match = false;
        break;
      }
    }
    if (match) return users[u].name;
  }
  return NULL;
}

void showResult(String line1, String line2, bool granted) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, 10);
  display.println(line1);

  display.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
  x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, 35);
  display.println(line2);

  display.display();

  if (granted) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    tone(BUZZER_PIN, 1500, 200);
  } else {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 400, 200);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}
