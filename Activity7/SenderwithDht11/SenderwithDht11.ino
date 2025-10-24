#include <DHT.h>
#include <SPI.h>
#include <mcp2515.h>

#define DHT_PIN 21
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

MCP2515 mcp2515(5);

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial connection
    
    Serial.println("=== DHT11 CAN TRANSMITTER ===");
    
    // Initialize DHT
    dht.begin();
    delay(2000);
    
    // Initialize CAN
    SPI.begin();
    
    Serial.println("Initializing CAN...");
    
    if (mcp2515.reset() != MCP2515::ERROR_OK) {
        Serial.println("❌ FAILED: MCP2515 reset");
        while(1);
    }
    Serial.println("✅ MCP2515 reset OK");
    
    if (mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK) {
        Serial.println("❌ FAILED: Set bitrate");
        while(1);
    }
    Serial.println("✅ Bitrate set OK");
    
    if (mcp2515.setNormalMode() != MCP2515::ERROR_OK) {
        Serial.println("❌ FAILED: Set normal mode");
        while(1);
    }
    Serial.println("✅ Normal mode set OK");
    
    Serial.println("🚀 TRANSMITTER READY!");
    Serial.println("=======================");
}

void loop() {
    // Read from DHT11
    float humidity = dht.readHumidity();
    float temperatureC = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperatureC)) {
        Serial.println("❌ DHT11 read failed!");
        delay(2000);
        return;
    }
    
    // Prepare CAN message
    struct can_frame canMsg;
    canMsg.can_id = 0x036;
    canMsg.can_dlc = 4;
    
    // Convert to integers
    int tempInt = (int)(temperatureC * 100);
    int humInt = (int)(humidity * 100);
    
    // Pack data
    canMsg.data[0] = (tempInt >> 8) & 0xFF;
    canMsg.data[1] = tempInt & 0xFF;
    canMsg.data[2] = (humInt >> 8) & 0xFF;
    canMsg.data[3] = humInt & 0xFF;
    
    // Send message
    byte result = mcp2515.sendMessage(&canMsg);
    
    if (result == MCP2515::ERROR_OK) {
        Serial.print("✅ SENT: Temp=");
        Serial.print(temperatureC);
        Serial.print("C, Hum=");
        Serial.print(humidity);
        Serial.println("%");
    } else {
        Serial.print("❌ SEND FAILED: Error ");
        Serial.println(result);
        
        // Diagnostic info
        Serial.println("Check:");
        Serial.println("1. MCP2515 VCC connected to 5V");
        Serial.println("2. SPI wiring correct");
        Serial.println("3. Both GND connected");
        Serial.println("4. CAN modules connected together");
    }
    
    delay(3000); // Send every 3 seconds
}