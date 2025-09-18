#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN    2
#define SS_PIN     5
#define BUZZER_PIN 15

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);


const byte AUTH_UID[] = { 0x42, 0xFA, 0x69, 0x05 };


bool isAuthorized(byte *uid, byte size) {
  if (size != sizeof(AUTH_UID)) return false;
  for (byte i = 0; i < size; i++)
    if (uid[i] != AUTH_UID[i]) return false;
  return true;
}

String uidToString(byte *uid, byte size) {
  String s = "";
  for (byte i = 0; i < size; i++) {
    if (uid[i] < 0x10) s += "0";
    s += String(uid[i], HEX) + (i < size - 1 ? " " : "");
  }
  s.toUpperCase();
  return s;
}

void beepOK()    { digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW); }
void beepError() { for (int i = 0; i < 3; i++) { digitalWrite(BUZZER_PIN, HIGH); delay(150); digitalWrite(BUZZER_PIN, LOW); delay(150); } }


void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(21, 22);
  lcd.init(); lcd.backlight();
  lcd.setCursor(0,0); lcd.print("HELLO BOSS");
  lcd.setCursor(0,1); lcd.print("SHOW YOUR CARD");

  SPI.begin(18, 19, 23, SS_PIN);
  rfid.PCD_Init();
  Serial.println("RFID Ready!");
}


void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String scanned = uidToString(rfid.uid.uidByte, rfid.uid.size);
  Serial.println("Scanned UID: " + scanned);

  lcd.clear();
  lcd.setCursor(0, 1); lcd.print("UID: " + scanned);

  if (isAuthorized(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("ACCESS GRANTED");
    lcd.setCursor(0, 0); lcd.print("ARREL'S CARD");
    beepOK();
  } else {
    Serial.println("WRONG CARD");
    lcd.setCursor(0, 0); lcd.print("WRONG CARD");
    beepError();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1500);
}
