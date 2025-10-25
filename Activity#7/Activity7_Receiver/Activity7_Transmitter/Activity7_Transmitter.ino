#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT11
#define CAN_CS 5
#define CAN_INT 4

MCP_CAN CAN0(CAN_CS);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Initializing CAN...");
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
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  byte data[8];
  memcpy(data, &t, 4);        
  memcpy(data + 4, &h, 4);   

  CAN0.sendMsgBuf(0x100, 0, 8, data);
  Serial.print("Sent -> Temp: ");
  Serial.print(t);
  Serial.print(" | Humid: ");
  Serial.println(h);

  delay(2000);
}