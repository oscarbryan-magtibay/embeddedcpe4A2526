#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 4;  

void setup() {
  Serial.begin(115200);

  SerialBT.begin("MAU_ESP32_Servo_Control");
  Serial.println("Bluetooth Device is Ready to Pair");

  myServo.attach(SERVO_PIN);
  myServo.write(0); 
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    int angle = input.toInt(); 

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      SerialBT.printf("Servo moved to %d degrees\n", angle);
    } 
    else {
      SerialBT.println("Invalid input! Enter a value between 0 and 180.");
    }
  }
}