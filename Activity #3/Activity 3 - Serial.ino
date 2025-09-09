#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 2;  

void setup() {
  Serial.begin(115200);

  SerialBT.begin("ESP32_REUBEN_Servo_Control");
  Serial.println("Bluetooth Device is Ready to Pair");

  myServo.attach(SERVO_PIN);
  myServo.write(0); // Start at 0 degrees
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    int angle = input.toInt(); // Convert received string to integer

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.printf("Servo moved to %d degrees\n", angle);
      SerialBT.printf("Servo moved to %d degrees\n", angle);
    } 
    else {
      Serial.println("Invalid input! Enter a value between 0 and 180.");
      SerialBT.println("Invalid input! Enter a value between 0 and 180.");
    }
  }
}