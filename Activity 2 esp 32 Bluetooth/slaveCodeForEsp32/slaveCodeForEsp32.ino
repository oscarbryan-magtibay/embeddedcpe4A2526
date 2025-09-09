#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 23; // GPIO pin for servo (change as needed)
int currentAngle = 0;    // Default position

void setup() {
  Serial.begin(115200);

  // Initialize Bluetooth in Slave mode
  SerialBT.begin("ESP32_Servo_Receiver"); 
  Serial.println("Servo Receiver ready. Waiting for Master...");

  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(currentAngle);
  Serial.println("Servo initialized at 90 degrees");
}

void loop() {
  if (SerialBT.available()) {
    String receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();
    
    // Convert to integer and validate
    int angle = receivedData.toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      currentAngle = angle;
      Serial.print("Servo moved to: ");
      Serial.print(angle);
      Serial.println(" degrees");
    } else {
      Serial.println("Invalid angle received");
    }
  }
  
  delay(50); // Small delay for stability
}