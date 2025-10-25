#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define SPI_CS_PIN 5
#define MCP_INT_PIN 26

DHT dht(DHTPIN, DHTTYPE);
MCP_CAN CAN(SPI_CS_PIN);

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  SPI.begin(18, 19, 23, SPI_CS_PIN);
  Serial.println("Init MCP2515 Sender...");

  if (CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 OK");
  } else {
    Serial.println("MCP2515 FAIL");
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

    uint8_t data[2] = {(uint8_t)t, (uint8_t)h};
    byte sendStat = CAN.sendMsgBuf(0x100, 0, 2, data);

    if (sendStat == CAN_OK) {
      Serial.printf("Sent T=%.1f°C, H=%.1f%%\n", t, h);
    } else {
      Serial.println("DHT DATA Send Failed!");
    }
  }
}