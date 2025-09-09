#include "BluetoothSerial.h"
#include <ESP32Servo.h>   

BluetoothSerial SerialBT;
Servo myServo;

const int SERVO_PIN = 4;  

void setup() {
  Serial.begin(115200);

 
  SerialBT.begin("ESP32_Servo_Receiver");
  Serial.println("Receiver ready. Waiting for Master...");


  myServo.setPeriodHertz(50);      
  myServo.attach(SERVO_PIN, 500, 2400); 


  myServo.write(0); 
}

void loop() {
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    int angle = input.toInt();

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.printf("Servo moved to %d degrees\n", angle);
    } else {
      Serial.printf("Invalid angle received: %s\n", input.c_str());
    }
  }
}
