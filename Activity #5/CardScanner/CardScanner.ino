#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN 27
#define SS_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Scan your card");
  display.display();

  SPI.begin(); 
  mfrc522.PCD_Init();
  Serial.println("Place your card near the reader...");
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.println();

  display.clearDisplay();
  display.setCursor(10, 25);
  display.println("Hello World!");
  display.display();

  delay(3000);

  display.clearDisplay();
  display.setCursor(10, 25);
  display.println("Scan your card");
  display.display();
}
