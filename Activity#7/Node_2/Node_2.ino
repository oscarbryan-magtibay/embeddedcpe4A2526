#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CAN_CS 5
#define CAN_INT 4
MCP_CAN CAN0(CAN_CS);

void setup() {
  Serial.begin(115200);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // CAN init
  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init failed, retrying...");
    delay(500);
  }
  Serial.println("CAN init OK!");
  CAN0.setMode(MCP_NORMAL);
}

void loop() {
  if (!digitalRead(CAN_INT)) {  // If data received
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    if (rxId == 0x123 && len == 4) {
      int temp10 = (rxBuf[0] << 8 | rxBuf[1];
      int hum10 = (rxBuf[2] << 8 | rxBuf[3];

      float temp = temp10 / 10.0;
      float hum = hum10 / 10.0;

      Serial.printf("Received → Temp: %.1f°C  Hum: %.1f%%\n", temp, hum);

      display.clearDisplay();
      display.setCursor(0, 10);
      display.printf("Temp: %.1f C\nHum:  %.1f %%", temp, hum);
      display.display();
    }
  }
}