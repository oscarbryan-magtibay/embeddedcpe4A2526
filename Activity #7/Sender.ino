#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

// Define Pin Constants
#define DHTPIN 4            // Pin for DHT sensor
#define DHTTYPE DHT11       // DHT sensor type (DHT11)
#define SPI_CS_PIN 5        // SPI Chip Select Pin for MCP2515
#define MCP_INT_PIN 26      // Interrupt Pin for MCP2515

// Initialize DHT sensor and CAN object
DHT dht(DHTPIN, DHTTYPE);        // DHT sensor object
MCP_CAN CAN(SPI_CS_PIN);         // MCP2515 CAN controller object

// Define timing for CAN message sending
unsigned long lastSend = 0;      // Variable to track last send time

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  dht.begin();  // Initialize DHT sensor

  // Initialize SPI for communication with MCP2515
  SPI.begin(18, 19, 23, SPI_CS_PIN);
  Serial.println("Initializing MCP2515 Sender...");

  // Initialize MCP2515 CAN controller
  if (CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully");
  } else {
    Serial.println("MCP2515 Initialization Failed");
  }

  // Set MCP2515 to normal mode for communication
  CAN.setMode(MCP_NORMAL);

  // Set interrupt pin for MCP2515
  pinMode(MCP_INT_PIN, INPUT_PULLUP);
}

void loop() {
  // Check if it's time to send a new CAN message (every 2 seconds)
  if (millis() - lastSend > 2000) {
    lastSend = millis(); // Update last send time

    // Read temperature and humidity from DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if there was an error reading the sensor
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error reading DHT sensor data");
      return;
    }

    // Prepare data for CAN message (temperature and humidity)
    uint8_t data[2] = {(uint8_t)temperature, (uint8_t)humidity};

    // Send CAN message with sensor data
    byte sendStatus = CAN.sendMsgBuf(0x100, 0, 2, data);

    // Check if message was sent successfully
    if (sendStatus == CAN_OK) {
      Serial.printf("Sent: Temperature = %.1f°C, Humidity = %.1f%%\n", temperature, humidity);
    } else {
      Serial.println("CAN Send Failed!");
    }
  }
}
