#include <SPI.h>
#include <mcp_can.h>
#include "DHT.h"

#define DHTPIN 17
#define DHTTYPE DHT11
#define CAN_CS 5
#define CAN_INT 4

DHT dht(DHTPIN, DHTTYPE);
MCP_CAN CAN(CAN_CS);

void setup() {
  Serial.begin(115200);
  dht.begin();

  while (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init failed, retrying...");
    delay(500);
  }
  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN Transmitter Ready!");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  byte data[8];
  memcpy(data, &t, 4);  
  memcpy(data + 4, &h, 4); 

  CAN.sendMsgBuf(0x100, 0, 8, data);

  Serial.printf("Sent → Temp: %.2f°C  Humidity: %.2f%%\n", t, h);
  delay(2000);
}
