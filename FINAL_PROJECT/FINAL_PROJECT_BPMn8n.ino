#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Configuration
const char* ssid = "amadeus ramos";
const char* password = "deus123";

// n8n Webhook Configuration
const char* n8nWebhook = "http://172.20.10.2:5678/webhook-test/bf2af272-f2a9-43bb-9027-dd5429b3d945";

// Heart Rate Sensor Pin
#define HEARTBEAT_PIN 34

// Variables
bool measuring = false;
bool resultShown = false;
bool wifiConnected = false;
unsigned long startTime;
unsigned long lastBeat;
int beatCount = 0;
int bpmResult = 0;
int baseline = 0;  // Changed from static inside loop

void setup() {
  Serial.begin(115200);
  Serial.println("HEART RATE MONITOR");
  Serial.println("==================");
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize HTTP client
  WiFiClient client;
  HTTPClient http;
}

void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void sendToN8N(int bpm) {
  if (!wifiConnected) {
    Serial.println("WiFi not connected. Cannot send data.");
    return;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  // Prepare JSON payload
  String jsonPayload = "{\"bpm\": " + String(bpm) + ", \"timestamp\": \"" + getTimeStamp() + "\"}";
  
  // Start HTTP connection
  http.begin(client, n8nWebhook);
  http.addHeader("Content-Type", "application/json");
  
  // Send POST request
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    Serial.print("Data sent to n8n successfully. Response code: ");
    Serial.println(httpResponseCode);
    
    String response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
  } else {
    Serial.print("Error sending data to n8n: ");
    Serial.println(httpResponseCode);
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }
  
  http.end();
}

String getTimeStamp() {
  // Get current time (simple implementation)
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  hours = hours % 24;
  
  // Format: HH:MM:SS
  char timestamp[12];
  sprintf(timestamp, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  
  return String(timestamp);
}

void loop() {
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }
  
  // Take multiple readings for better accuracy
  int val1 = analogRead(HEARTBEAT_PIN);
  delay(2);
  int val2 = analogRead(HEARTBEAT_PIN);
  delay(2);
  int val3 = analogRead(HEARTBEAT_PIN);
  int val = (val1 + val2 + val3) / 3;
  
  // Start measurement when finger is placed
  if (val > 1500 && !measuring && !resultShown) {
    measuring = true;
    resultShown = false;
    startTime = millis();
    lastBeat = 0;
    beatCount = 0;
    bpmResult = 0;
    baseline = val;  // Initialize baseline with current reading
    
    Serial.println("\nFINGER DETECTED");
    Serial.print("Initial reading: ");
    Serial.println(val);
    
    // Shorter stabilization time
    for (int i = 0; i < 20; i++) {
      delay(100);
      int tempVal = analogRead(HEARTBEAT_PIN);
      baseline = (baseline + tempVal) / 2;  // Smooth baseline initialization
    }
    Serial.print("Baseline stabilized: ");
    Serial.println(baseline);
  }
  
  // Stop if finger removed
  if (val < 1000 && measuring) {
    measuring = false;
    if (!resultShown) {
      Serial.println("\nMEASUREMENT STOPPED");
    }
  }
  
  // Perform measurement
  if (measuring && !resultShown) {
    static int lastValue = val;
    
    // Update baseline slowly
    if (abs(val - baseline) < 500) {
      baseline = (baseline * 9 + val) / 10;  // Slower baseline update
    }
    
    int diff = val - baseline;
    
    // Debug output (uncomment for troubleshooting)
    // Serial.print("val: "); Serial.print(val);
    // Serial.print(" baseline: "); Serial.print(baseline);
    // Serial.print(" diff: "); Serial.println(diff);
    
    // Simple beat detection
    static bool wasHigh = false;
    
    // Detect heartbeat - adjusted threshold
    if (diff > 10 && !wasHigh) {  // Lowered threshold from 20 to 10
      unsigned long now = millis();
      
      if (lastBeat > 0) {
        unsigned long interval = now - lastBeat;
        
        // Valid beat interval
        if (interval > 300 && interval < 2000) {
          beatCount++;
          
          // Show heart symbol only
          Serial.print("♥ ");
          
          // Calculate BPM after 4 beats
          if (beatCount >= 4) {
            // Store last few intervals
            static unsigned long intervals[5];
            static int idx = 0;
            
            intervals[idx] = interval;
            idx = (idx + 1) % 5;
            
            // Calculate average of valid intervals
            long total = 0;
            int validCount = 0;
            for (int i = 0; i < 5; i++) {
              if (intervals[i] > 300 && intervals[i] < 2000) {
                total += intervals[i];
                validCount++;
              }
            }
            
            if (validCount >= 3) {
              int avgInterval = total / validCount;
              bpmResult = 60000 / avgInterval;
              
              // Validate BPM
              if (bpmResult >= 35 && bpmResult <= 200) {
                resultShown = true;
                measuring = false;
                
                // Show final result
                Serial.println("\n====================");
                Serial.print("BPM: ");
                Serial.println(bpmResult);
                Serial.println("====================");
                
                // Send BPM data to n8n webhook
                Serial.println("\nSending data to n8n...");
                sendToN8N(bpmResult);
              }
            }
          }
        }
      }
      
      lastBeat = now;
      wasHigh = true;
    }
    
    if (diff < 5) wasHigh = false;
    
    // Timeout after 30 seconds (increased from 20)
    if (millis() - startTime > 30000 && !resultShown) {
      resultShown = true;
      measuring = false;
      Serial.println("\nNO HEARTBEAT DETECTED");
      Serial.print("Last diff: ");
      Serial.println(diff);
      Serial.print("Last baseline: ");
      Serial.println(baseline);
    }
  }
  
  // Reset for next measurement when finger removed
  if (val < 1000 && resultShown) {
    resultShown = false;
    delay(1000);
    Serial.println("\nREADY");
  }
  
  delay(30);
}