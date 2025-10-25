#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CAN_CS_PIN 5     // D5 on some boards
#define CAN_INT_PIN 4    // D4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

MCP_CAN CAN(CAN_CS_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // SPI pins (SCK, MISO, MOSI, CS)
  SPI.begin(18, 19, 23, CAN_CS_PIN);

  // I2C for OLED (SDA, SCL)
  Wire.begin(21, 22);

  // OLED setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed!");
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
    Serial.println("MCP2515 Initialized OK");
    display.println("MCP2515 OK");
  } else {
    Serial.println("MCP2515 Initialization Failed");
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
  if (digitalRead(CAN_INT_PIN) == LOW) {
    unsigned long rxId;
    byte len;
    byte buf[8];

    CAN.readMsgBuf(&rxId, &len, buf);

    if (len >= 2) {
      float temp = buf[0];
      float hum = buf[1];

      Serial.printf("CAN RX (ID 0x%lX): Temp=%.1f°C, Hum=%.1f%%\n", rxId, temp, hum);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("CAN Receiver");
      display.println("----------------");
      display.print("Temp: "); display.print(temp, 1); display.println(" °C");
      display.print("Hum:  "); display.print(hum, 1); display.println(" %");
      display.display();
    }
  }
  delay(100);
}