#include <SPI.h>
#include <mcp_can.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define CAN_CS 5
#define CAN_INT 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 21
#define OLED_SCL 22

MCP_CAN CAN0(CAN_CS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float t = 0.0, h = 0.0;

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Waiting for CAN...");
  display.display();

  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("CAN Initialized Successfully!");
  else {
    Serial.println("CAN Init Failed!");
    while (1);
  }

  CAN0.setMode(MCP_NORMAL);
  Serial.println("CAN set to NORMAL mode.");
}

void loop() {
  unsigned char len = 0;
  unsigned char buf[8];
  long unsigned int rxId;

  bool canDetected = false;

  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    CAN0.readMsgBuf(&rxId, &len, buf);
    canDetected = true;

    if (rxId == 0x100 && len == 😎 {
      memcpy(&t, buf, 4);
      memcpy(&h, buf + 4, 4);

      Serial.print("Received -> Temp: ");
      Serial.print(t);
      Serial.print(" | Humid: ");
      Serial.println(h);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DHT11 + CAN Monitor");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(t);
  display.println(" C");
  display.setCursor(0, 28);
  display.print("Humid: ");
  display.print(h);
  display.println(" %");
  display.setCursor(0, 44);
  display.print("CAN: ");
  display.println(canDetected ? "Detected" : "No Activity");
  display.display();

  delay(500);
}