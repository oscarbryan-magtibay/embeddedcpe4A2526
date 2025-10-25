#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Define I2C Address for LCD and LCD Dimensions
#define I2C_ADDR 0x27   
#define LCD_COLUMNS 20
#define LCD_ROWS 4

// Initialize LCD object
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

// Define RFID Module Pins
#define SS_PIN 5  
#define RST_PIN 2   

// Initialize MFRC522 RFID object
MFRC522 rfid(SS_PIN, RST_PIN);

// Define a unique RFID UID for a specific card (e.g., a random UID)
byte customUID[] = {0x12, 0x34, 0x56, 0x78}; // Modify this for your unique card

// Setup function to initialize the LCD and RFID
void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Display welcome message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Please tap your");
  lcd.setCursor(0, 1);
  lcd.print("card");

  // Initialize SPI and RFID module
  SPI.begin();
  rfid.PCD_Init();
  
  // Wait for a moment to ensure the system is ready
  delay(1000);
}

// Function to check if the scanned RFID UID matches the defined UID
bool isMatchingCard(byte *uid, byte uidSize) {
  // Ensure the UID size matches the defined size
  if (uidSize != sizeof(customUID)) return false;
  
  // Compare each byte of the UID with the custom UID
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != customUID[i]) return false;
  }
  return true;
}

// Main loop to continuously check for RFID input
void loop() {
  // Check if a new card is present and read its UID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return; // If no card is detected, exit the loop
  }

  // If the UID matches the predefined custom UID
  if (isMatchingCard(rfid.uid.uidByte, rfid.uid.size)) {
    // Display "Access Granted" message on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome, Authorized");
    lcd.setCursor(0, 1);
    lcd.print("User");

  } else {
    // Display "Access Denied" message on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
  }

  // Wait for 2 seconds to show the message
  delay(2000); 
  
  // Clear the LCD and prompt the user to tap their card again
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please tap your");
  lcd.setCursor(0, 1);
  lcd.print("card");

  // Halt communication with the card and stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
