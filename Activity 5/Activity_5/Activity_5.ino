#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>


#define I2C_ADDR 0x27   
#define LCD_COLUMNS 20
#define LCD_ROWS 4
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);


#define SS_PIN 5  
#define RST_PIN 2   
MFRC522 rfid(SS_PIN, RST_PIN);


byte queenieUID[] = {0xDE, 0xAD, 0xBE, 0xEF}; 

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Please tap your");
  lcd.setCursor(0, 1);
  lcd.print("card");


  SPI.begin();
  rfid.PCD_Init();
  delay(1000);
}


bool isQueenieCard(byte *uid, byte uidSize) {
  if (uidSize != sizeof(queenieUID)) return false;
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != queenieUID[i]) return false;
  }
  return true;
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;  
  }


  if (isQueenieCard(rfid.uid.uidByte, rfid.uid.size)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome Queenie");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
  }

  delay(2000); // 

 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please tap your");
  lcd.setCursor(0, 1);
  lcd.print("card");


  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}