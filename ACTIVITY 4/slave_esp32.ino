#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial BTReceiver;
Servo servo;
const int servoPin = 23;

void setup() {
  Serial.begin(115200);
  BTReceiver.begin("ESP32_Receiver");
  servo.attach(servoPin);
  servo.write(90);
  Serial.println("Receiver initialized, waiting for data...");
}

void loop() {
  if (BTReceiver.available()) {
    String data = BTReceiver.readStringUntil('\n');
    data.trim();
    int angle = data.toInt();
    if (angle >= 0 && angle <= 180) {
      servo.write(angle);
      Serial.print("Servo angle set to: ");
      Serial.println(angle);
    } else {
      Serial.print("Invalid angle received: ");
      Serial.println(data);
    }
  }
}
