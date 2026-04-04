#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define CAN_CS 5
#define CAN_INT 2

MCP_CAN CAN0(CAN_CS);

// Change 0x27 if your LCD has different address (sometimes 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // LCD init
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Waiting data...");

  // CAN init
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN Init OK");
  } else {
    Serial.println("CAN Init FAILED");
    while (1);
  }

  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN_INT, INPUT);
}

void loop() {
  if (!digitalRead(CAN_INT)) {
    long unsigned int rxId;
    byte len;
    byte rxBuf[8];

    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    if (rxId == 0x100 && len >= 2) {
      byte temp = rxBuf[0];
      byte hum  = rxBuf[1];

      Serial.print("Received -> Temp: ");
      Serial.print(temp);
      Serial.print(" C | Hum: ");
      Serial.print(hum);
      Serial.println(" %");

      // Display on LCD
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temp);
      lcd.print(" C");

      lcd.setCursor(0, 1);
      lcd.print("Hum : ");
      lcd.print(hum);
      lcd.print(" %");
    }
  }
}
