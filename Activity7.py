#Arduino SENDER

/*
 * NODE 1: SENDER (DHT11 Sensor + MCP2515)
 * - Uses SPI to communicate with the MCP2515
 */

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
// Note: Set speed to 500KBPS and 8MHZ crystal (most common on blue/red modules)
#define CAN_SPEED     CAN_500KBPS
#define CAN_CLOCK     MCP_8MHZ

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

#Arduino RECEIVER
/*
 * NODE 2: RECEIVER (OLED Display + MCP2515)
 * - Uses SPI to communicate with the MCP2515
 * - Uses Interrupt pin to detect new messages
 */

#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions ---
#define CS_PIN        5   // SPI Chip Select
#define INT_PIN       2   // Interrupt pin from MCP2515

// --- CAN Definitions ---
#define CAN_ID        0x123  // Message ID to listen for
#define CAN_SPEED     CAN_500KBPS
#define CAN_CLOCK     MCP_8MHZ

// --- OLED Definitions ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MCP2515 mcp2515(CS_PIN);
struct can_frame rx_frame;

// --- Global Variables ---
float temperature = 0.0;
float humidity = 0.0;
bool isConnected = false;
unsigned long lastMsgTime = 0;

const long COMMUNICATION_TIMEOUT = 2500; // 2.5 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("== ESP32 MCP2515 Receiver (OLED) ==");

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Waiting for CAN data...");
  display.display();

  // Initialize SPI
  SPI.begin();

  // Initialize MCP2515
  mcp2515.reset();
  if (mcp2515.setBitrate(CAN_SPEED, CAN_CLOCK) != MCP2515::ERROR_OK) {
    Serial.println("Error setting CAN bit rate!");
    while (1);
  }
  
  // Set filter to only receive our specific ID
  // This tells the MCP2515 to only interrupt us for this ID
  mcp2515.setFilterMask(MCP2515::MASK0, false, 0x7FF);
  mcp2515.setFilter(MCP2515::RXF0, false, CAN_ID);
  
  mcp2515.setNormalMode();
  
  // Setup the interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  
  Serial.println("MCP2515 Initialized. Listening...");
}

void loop() {
  checkCANMessages();
  checkConnectionStatus();
  updateDisplay();
}

void checkCANMessages() {
  // The INT pin goes LOW when a message is received
  if (digitalRead(INT_PIN) == LOW) {
    
    // Read the message
    if (mcp2515.readMessage(&rx_frame) == MCP2515::ERROR_OK) {
      
      // Double-check if it's the ID we care about
      if (rx_frame.can_id == CAN_ID && rx_frame.can_dlc == 8) {
        // We received a message! Reset the timeout timer.
        lastMsgTime = millis();
        isConnected = true;

        // Unpack the data from the frame
        memcpy(&temperature, &rx_frame.data[0], 4); // Get temp from bytes 0-3
        memcpy(&humidity, &rx_frame.data[4], 4);    // Get humi from bytes 4-7

        Serial.print("Received CAN frame. | Temp: ");
        Serial.print(temperature);
        Serial.print(" C, Humi: ");
        Serial.print(humidity);
        Serial.println(" %");
      }
    }
  }
}

void checkConnectionStatus() {
  // This is our "detection of CAN communication"
  if (millis() - lastMsgTime > COMMUNICATION_TIMEOUT) {
    if (isConnected) {
      Serial.println("CAN Communication LOST!");
      isConnected = false;
    }
  }
}

void updateDisplay() {
  static unsigned long lastDisplayTime = 0;
  if (millis() - lastDisplayTime < 500) {
    return; // Update display only every 500ms
  }
  lastDisplayTime = millis();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2); 

  if (isConnected) {
    display.println("ONLINE");
    display.setTextSize(1);
    display.println("---------------------");
    display.setTextSize(2);
    display.print("T: ");
    display.print(temperature, 1);
    display.println(" C");
    
    display.print("H: ");
    display.print(humidity, 1);
    display.println(" %");

  } else {
    display.println("OFFLINE");
    display.setTextSize(1);
    display.println("---------------------");
    display.setCursor(0, 30);
    display.println("No CAN data received.");
  }
  
  display.display();
}