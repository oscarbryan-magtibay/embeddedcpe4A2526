#include "BluetoothSerial.h"

#define SERVO_PIN 13
#define SERVO_CHANNEL 0 
#define SERVO_FREQ 50 
#define SERVO_RES 16 

BluetoothSerial SerialBT;

int currentAngle = 90;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_ServoReceiver");
  Serial.println("Receiver ready. Waiting for Master...");

  ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);

  writeServo(currentAngle);
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        currentAngle = angle;
        writeServo(currentAngle);
        Serial.print("Servo moved to: ");
        Serial.println(currentAngle);
      }
    }
  }
}

void writeServo(int angle) {
  int minUs = 500;   // pulse for 0°
  int maxUs = 2400;  // pulse for 180°
  int us = map(angle, 0, 180, minUs, maxUs);
  int duty = (int)((us / 20000.0) * 65535); // para 50Hz
  ledcWrite(SERVO_CHANNEL, duty);
}