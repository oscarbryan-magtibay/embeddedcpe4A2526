#include <SPI.h>
#include <Wire.h>
#include <mcp_can.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CAN_CS 5
#define CAN_INT 4
MCP_CAN CAN0(CAN_CS);

#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 17

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CAN_CS);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("CAN Receiver Ready!");
  display.display();
  delay(1000);

  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init failed, retrying...");
    delay(500);
  }
  CAN0.setMode(MCP_NORMAL);
  Serial.println("CAN init OK!");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("CAN init OK!");
  display.display();
  delay(1000);
}

void loop() {
  long unsigned int rxId;
  unsigned char len = 0;
  unsigned char rxBuf[8];

  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    if (rxId == 0x100 && len == 8) {
      float temp, hum;
      memcpy(&temp, rxBuf, 4);
      memcpy(&hum, rxBuf + 4, 4);

      Serial.printf("Received → Temp: %.2f°C  Hum: %.2f%%\n", temp, hum);

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10);
      display.printf("T:%.1fC", temp);
      display.setCursor(0, 40);
      display.printf("H:%.1f%%", hum);
      display.display();
    }
  }
}
