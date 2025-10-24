#include <SPI.h>
#include <Wire.h>
#include <mcp_can.h>
#include <LiquidCrystal_I2C.h>

// -----------------------------
// CAN BUS SETUP
// -----------------------------
#define CAN_CS 5
#define CAN_INT 4
MCP_CAN CAN0(CAN_CS);

// Custom SPI pins for your board
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 17  // Changed from 23 → 33 for your ESP32

// -----------------------------
// LCD 16x2 I2C SETUP
// -----------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -----------------------------
// LED SETUP
// -----------------------------
#define LED_GREEN 25
#define LED_RED 26
#define LED_BLUE 27

// -----------------------------
// CAN MESSAGE IDs
// -----------------------------
#define CAN_ID_DATA 0x036
#define CAN_ID_ACK  0x037

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CAN_CS);

  // Initialize LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CAN Monitor Ready");

  delay(1000);

  // Initialize CAN at 500 kbps
  Serial.println("Initializing CAN...");
  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {  
    // 🔁 Change to MCP_16MHZ if your MCP2515 crystal says “16.000”
    Serial.println("CAN init failed, retrying...");
    delay(500);
  }

  CAN0.setMode(MCP_NORMAL);
  Serial.println("CAN init OK!");
}

void loop() {
  // Check if data was received
  if (!digitalRead(CAN_INT)) {
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    // Process incoming message
    if (rxId == CAN_ID_DATA && len == 4) {
      int tempInt = (rxBuf[0] << 8) | rxBuf[1];
      int humInt  = (rxBuf[2] << 8) | rxBuf[3];

      float temp = tempInt / 100.0;
      float hum  = humInt  / 100.0;

      Serial.printf("Received → Temp: %.2f°C  Hum: %.2f%%\n", temp, hum);

      // Display on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.printf("Temp: %.2f C", temp);
      lcd.setCursor(0, 1);
      lcd.printf("Hum:  %.2f %%", hum);

      // LED Status Logic
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_BLUE, LOW);

      if (temp > 30.0) {
        digitalWrite(LED_RED, HIGH);   // Overheat alert
      } else if (hum > 70.0) {
        digitalWrite(LED_BLUE, HIGH);  // High humidity alert
      } else {
        digitalWrite(LED_GREEN, HIGH); // Normal condition
      }

      // Send ACK to sender
      byte dummy[1] = {0};
      if (CAN0.sendMsgBuf(CAN_ID_ACK, 0, 0, dummy) == CAN_OK) {
        Serial.println("ACK sent ✅");
      } else {
        Serial.println("Failed to send ACK ❌");
      }
    }
  }
}
