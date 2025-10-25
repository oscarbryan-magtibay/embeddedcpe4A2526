#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define SPI_CS_PIN 5
#define MCP_INT_PIN 2  // changed from 26

DHT dht(DHTPIN, DHTTYPE);
MCP_CAN CAN(SPI_CS_PIN);

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize SPI with default ESP32 pins
  SPI.begin(18, 19, 23, SPI_CS_PIN);
  Serial.println("Init MCP2515 Sender...");

  // Initialize MCP2515
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 OK");
  } else {
    Serial.println("MCP2515 FAIL");
    while(1); // halt if CAN fails
  }

  CAN.setMode(MCP_NORMAL);
  pinMode(MCP_INT_PIN, INPUT_PULLUP);
}

void loop() {
  if (millis() - lastSend > 2000) {
    lastSend = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("DHT error");
      return;
    }

    // Pack into 4 bytes (integer + decimal) for better precision
    uint8_t data[4];
    data[0] = (uint8_t)t;                      // integer part of temperature
    data[1] = (uint8_t)((t - (int)t) * 100);  // decimal part of temperature
    data[2] = (uint8_t)h;                      // integer part of humidity
    data[3] = (uint8_t)((h - (int)h) * 100);  // decimal part of humidity

    byte sendStat = CAN.sendMsgBuf(0x100, 0, 4, data);

    if (sendStat == CAN_OK) {
      Serial.printf("Sent T=%.2f°C, H=%.2f%%\n", t, h);
    } else {
      Serial.println("CAN Send Failed!");
    }
  }
}
