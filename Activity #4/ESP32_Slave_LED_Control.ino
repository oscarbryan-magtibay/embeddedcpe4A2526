#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 2;  // Use a PWM-capable pin on ESP32

void setup() {
  Serial.begin(115200);

  // Initialize Bluetooth in Slave mode
  SerialBT.begin("ESP32_Servo_Receiver");
  Serial.println("Receiver ready. Waiting for Master...");

  // Attach servo
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Start at 0 degrees
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n'); // Receive angle
    input.trim();
    int angle = input.toInt();

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.printf("Servo moved to %d degrees\n", angle);
    } else {
      Serial.println("Invalid input received");
    }
  }
}
