#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MCP2515 mcp2515(5);

unsigned long lastUpdate = 0;
bool dataReceived = false;
float lastTemperature = 0.0;
float lastHumidity = 0.0;
int messageCount = 0;
bool oledInitialized = false;

void setup() {
    Serial.begin(115200);
    Serial.println("=== CAN RECEIVER STARTING ===");
    
    // Initialize I2C
    Wire.begin();
    delay(1000);
    
    // Initialize OLED
    Serial.println("Initializing OLED...");
    oledInitialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    if(!oledInitialized) {
        Serial.println("0x3C failed, trying 0x3D...");
        oledInitialized = display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    }
    
    if(oledInitialized) {
        Serial.println("✅ OLED initialized successfully");
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        
        // Show initial screen
        display.setTextSize(2);
        display.setCursor(15, 25);
        display.println("READY");
        display.display();
    } else {
        Serial.println("❌ OLED initialization failed!");
    }
    
    // Initialize CAN
    SPI.begin();
    
    if (mcp2515.reset() != MCP2515::ERROR_OK) {
        Serial.println("❌ MCP2515 reset failed");
        return;
    }
    
    if (mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK) {
        Serial.println("❌ Bitrate set failed");
        return;
    }
    
    if (mcp2515.setNormalMode() != MCP2515::ERROR_OK) {
        Serial.println("❌ Normal mode failed");
        return;
    }
    
    Serial.println("🎯 RECEIVER READY - Listening for CAN messages...");
}

void updateDisplay(float temp, float hum) {
    if(!oledInitialized) return;
    
    display.clearDisplay();
    
    // Temperature Section - Top half
    display.setTextSize(1);
    display.setCursor(5, 2);
    display.print("TEMPERATURE");
    
    display.setTextSize(2);
    display.setCursor(5, 15);
    display.print(temp, 1);
    display.setTextSize(1);
    display.print(" C");
    
    // Line separator
    display.drawLine(0, 35, 128, 35, SSD1306_WHITE);
    
    // Humidity Section - Bottom half
    display.setTextSize(1);
    display.setCursor(5, 40);
    display.print("HUMIDITY");
    
    display.setTextSize(2);
    display.setCursor(5, 50);
    display.print(hum, 1);
    display.setTextSize(1);
    display.print(" %");
    
    display.display();
}

void loop() {
    struct can_frame canMsg;
    
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        messageCount++;
        Serial.print("📨 Message #");
        Serial.print(messageCount);
        Serial.print(" - ID: 0x");
        Serial.print(canMsg.can_id, HEX);
        Serial.print(", Data: ");
        
        for (int i = 0; i < canMsg.can_dlc; i++) {
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        
        if (canMsg.can_id == 0x036 && canMsg.can_dlc == 4) {
            int tempInt = (canMsg.data[0] << 8) | canMsg.data[1];
            int humInt = (canMsg.data[2] << 8) | canMsg.data[3];
            
            lastTemperature = tempInt / 100.0;
            lastHumidity = humInt / 100.0;
            dataReceived = true;
            lastUpdate = millis();
            
            Serial.print("✅ DATA: Temp=");
            Serial.print(lastTemperature);
            Serial.print("C, Hum=");
            Serial.print(lastHumidity);
            Serial.println("%");
            
            // Update OLED
            updateDisplay(lastTemperature, lastHumidity);
        }
    }
    
    delay(100);
}