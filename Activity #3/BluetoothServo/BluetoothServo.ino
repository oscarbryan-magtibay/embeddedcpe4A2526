#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo bluetoothServo;

const int SERVO_PIN = 2;
const float SERVO_SPEED = 180.0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo");
  Serial.println("Bluetooth device is ready to pair");

  bluetoothServo.attach(SERVO_PIN, 1000, 2000);
  bluetoothServo.write(90);
}

void rotateDegrees(int angle) {
  int direction = (angle >= 0) ? 180 : 0;
  int degrees = abs(angle);

  int duration = (degrees / SERVO_SPEED) * 1000;

  bluetoothServo.write(direction);
  delay(duration);
  bluetoothServo.write(90);
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    int angle = input.toInt();

    if (angle >= -180 && angle <= 180) {
      rotateDegrees(angle);
      Serial.printf("Rotated %d degrees\n", angle);
      SerialBT.printf("Rotated %d degrees\n", angle);
    }
    
    else {
      Serial.println("Invalid input! Enter 0 to 180.");
      SerialBT.println("Invalid input! Enter 0 to 180.");
    }
  }
}
