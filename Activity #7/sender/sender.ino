#include <SPI.h>
#include <mcp2515.h>
#include "DHT.h"

// --- Pin Definitions ---
#define DHT_PIN       15
#define DHT_TYPE      DHT11
#define CS_PIN        5   // SPI Chip Select for MCP2515
#define INT_PIN       2   // Interrupt pin (not used for sending, but good to define)

// --- CAN Definitions ---
#define CAN_ID        0x123  // Message ID for our sensor data
#define CAN_SPEED     CAN_500KBPS

// !!--CHECK THIS--!!
// This MUST match the crystal on your MCP2515 module.
// If you get "Error setting CAN bit rate!", change this.
// Common values are MCP_8MHZ or MCP_16MHZ.
#define CAN_CLOCK     MCP_16MHZ

// --- Global Objects ---
DHT dht(DHT_PIN, DHT_TYPE);
MCP2515 mcp2515(CS_PIN);      // Create MCP2515 object
struct can_frame canMsg;    // Create a CAN frame structure

void setup() {
  Serial.begin(115200);
  Serial.println("== ESP32 MCP2515 Sender (DHT11) ==");

  // Initialize DHT sensor
  dht.begin();

  // Initialize SPI
  SPI.begin();

  // Initialize MCP2515
  mcp2515.reset();
  if (mcp2515.setBitrate(CAN_SPEED, CAN_CLOCK) != MCP2515::ERROR_OK) {
    Serial.println("Error setting CAN bit rate!");
    while (1);
  }
  mcp2515.setNormalMode();
 
  Serial.println("MCP2515 Initialized.");

  // Prepare the CAN frame (ID and DLC can be set once)
  canMsg.can_id  = CAN_ID;
  canMsg.can_dlc = 8; // 8 bytes (4 for temp, 4 for humi)
}

void loop() {
  // Wait 2 seconds between readings
  delay(2000);

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Reads in Celsius

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Pack the float values into the 8-byte data array
  // `memcpy` is the safest way to do this
  memcpy(&canMsg.data[0], &temperature, 4); // Put temp in bytes 0-3
  memcpy(&canMsg.data[4], &humidity, 4);    // Put humi in bytes 4-7

  // --- Send CAN Frame ---
  MCP2515::ERROR result = mcp2515.sendMessage(&canMsg);
 
  if (result == MCP2515::ERROR_OK) {
    Serial.print("Sent CAN frame. ID: 0x");
    Serial.print(canMsg.can_id, HEX);
    Serial.print(" | Temp: ");
    Serial.print(temperature);
    Serial.print(" C, Humi: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.print("Error sending CAN frame! Code: ");
    Serial.println(result);
  }
}