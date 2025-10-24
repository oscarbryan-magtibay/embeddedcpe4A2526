#include <mcp_can.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define CAN_CS 5
#define CAN_INT 4

MCP_CAN CAN0(CAN_CS); // Set CS pin

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize CAN at 500 kbps
  while (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN init failed, retrying...");
    delay(500);
  }
  Serial.println("CAN init OK!");
  CAN0.setMode(MCP_NORMAL); // Normal mode to send data
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read failed!");
    delay(2000);
    return;
  }

  // Convert floats to integers (multiply by 10 to keep one decimal)
  int temp10 = (int)(temp * 10);
  int hum10 = (int)(hum * 10);

  byte data[4];
  data[0] = highByte(temp10);
  data[1] = lowByte(temp10);
  data[2] = highByte(hum10);
  data[3] = lowByte(hum10);

  byte sendStatus = CAN0.sendMsgBuf(0x123, 0, 4, data);

  if (sendStatus == CAN_OK) {
    Serial.printf("Sent → Temp: %.1f°C  Hum: %.1f%%\n", temp, hum);
  } else {
    Serial.println("Error sending message!");
  }

  delay(2000);
}