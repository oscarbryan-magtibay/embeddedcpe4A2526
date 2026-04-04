#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

#define CAN_CS 5
#define CAN_INT 2

#define DHTPIN 4
#define DHTTYPE DHT11

MCP_CAN CAN0(CAN_CS);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  // CAN init
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN Init OK");
  } else {
    Serial.println("CAN Init FAILED");
    while (1);
  }

  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN_INT, INPUT);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT read failed");
    delay(2000);
    return;
  }

  byte temp = (byte)temperature;
  byte hum  = (byte)humidity;

  byte data[2] = {temp, hum};

  CAN0.sendMsgBuf(0x100, 0, 2, data);

  Serial.print("Sent -> Temp: ");
  Serial.print(temp);
  Serial.print(" C | Hum: ");
  Serial.print(hum);
  Serial.println(" %");

  delay(2000);
}
