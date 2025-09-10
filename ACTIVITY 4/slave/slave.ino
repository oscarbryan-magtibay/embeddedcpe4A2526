#include "BluetoothSerial.h"
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myServo;

int servoPin = 18;  
int angle = 0;

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);

  SerialBT.begin("ESP32_Servo_Slave"); 
  Serial.println("Slave ready. Waiting for Master...");
}

void loop() {
  if (SerialBT.available()) {
    angle = SerialBT.parseInt();  
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid angle received.");
    }
  }
}
