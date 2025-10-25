#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define Pin Constants
#define CAN_CS_PIN 5     // CAN Chip Select Pin
#define CAN_INT_PIN 4    // CAN Interrupt Pin
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height
#define OLED_ADDR 0x3C   // OLED display I2C address

// Initialize CAN and OLED display
MCP_CAN CAN(CAN_CS_PIN);                 // Create CAN object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Create OLED display object

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  delay(1000);

  // Initialize SPI for CAN Bus communication
  SPI.begin(18, 19, 23, CAN_CS_PIN);

  // Initialize I2C for OLED display
  Wire.begin(21, 22);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Error initializing OLED display.");
    while (1); // Halt execution if initialization fails
  }

  // Display startup message on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing CAN Receiver...");
  display.display();

  // Initialize MCP2515 CAN controller
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized successfully.");
    display.println("MCP2515 Initialized");
  } else {
    Serial.println("Failed to initialize MCP2515.");
    display.println("MCP2515 Initialization Failed");
    display.display();
    while (1); // Halt execution if initialization fails
  }

  // Set CAN controller to Normal mode
  CAN.setMode(MCP_NORMAL);
  pinMode(CAN_INT_PIN, INPUT_PULLUP); // Set interrupt pin to input with pull-up

  // Final display setup
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CAN Receiver Ready");
  display.display();
}

void loop() {
  // Check if CAN interrupt signal is triggered (new message received)
  if (digitalRead(CAN_INT_PIN) == LOW) {
    unsigned long rxId;
    byte len;
    byte buf[8];

    // Read CAN message
    CAN.readMsgBuf(&rxId, &len, buf);

    // If message contains data (minimum 2 bytes)
    if (len >= 2) {
      float temperature = buf[0]; // Temperature data (byte)
      float humidity = buf[1];    // Humidity data (byte)

      // Output received data to Serial monitor
      Serial.printf("CAN RX (ID 0x%lX): Temp = %.1f°C, Humidity = %.1f%%\n", rxId, temperature, humidity);

      // Display received data on OLED screen
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("CAN Receiver");
      display.println("----------------");
      display.print("Temp: "); display.print(temperature, 1); display.println(" °C");
      display.print("Hum:  "); display.print(humidity, 1); display.println(" %");
      display.display();
    }
  }

  delay(100); // Short delay for better performance
}
