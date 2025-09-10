#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Servo_Slave");

  myServo.setPeriodHertz(50);    
  myServo.attach(13, 500, 2400);

  myServo.write(90); 
  Serial.println("Slave ready. Waiting for Master...");
}

void loop() {
  if (SerialBT.available()) {
    String data = SerialBT.readStringUntil('\n');
    int angle = data.toInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    }
  }
}
