#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CAN_CS_PIN 5      // MCP2515 CS
#define CAN_INT_PIN 2     // MCP2515 INT
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

MCP_CAN CAN(CAN_CS_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long lastMsgTime = 0;
bool hasData = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize SPI for MCP2515
  SPI.begin(18, 19, 23, CAN_CS_PIN);

  // Initialize I2C for OLED
  Wire.begin(21, 22);

  // OLED setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("❌ OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CAN Receiver Booting...");
  display.display();

  // MCP2515 setup
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("✅ MCP2515 Initialized OK");
    display.println("MCP2515 OK");
  } else {
    Serial.println("❌ MCP2515 Initialization Failed");
    display.println("MCP2515 FAIL");
    display.display();
    while (1);
  }

  CAN.setMode(MCP_NORMAL);
  pinMode(CAN_INT_PIN, INPUT_PULLUP);

  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CAN Receiver Ready");
  display.display();
}

void loop() {
  // Check if MCP2515 has received a message
  if (digitalRead(CAN_INT_PIN) == LOW) {
    unsigned long rxId;
    byte len;
    byte buf[8];

    if (CAN.readMsgBuf(&rxId, &len, buf) == CAN_OK) {
      if (len >= 4) {
        // Extract temperature and humidity from 4-byte data
        float temp = buf[0] + (buf[1] / 100.0);
        float hum  = buf[2] + (buf[3] / 100.0);

        Serial.printf("CAN RX (ID 0x%lX): Temp=%.2f°C, Hum=%.2f%%\n", rxId, temp, hum);

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("CAN Receiver");
        display.println("----------------");
        display.printf("Temp: %.2f °C\n", temp);
        display.printf("Hum:  %.2f %%", hum);
        display.display();

        lastMsgTime = millis();
        hasData = true;
      }
    }
  }

  // Show "No Signal" if no message for 5 seconds
  if (hasData && millis() - lastMsgTime > 5000) {
    display.clearDisplay();
    display.setCursor(20, 25);
    display.println("No CAN Signal");
    display.display();
    hasData = false;
  }

  delay(100);
}
