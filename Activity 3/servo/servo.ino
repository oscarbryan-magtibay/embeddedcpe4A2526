#include <ESP32Servo.h>

Servo myServo;
int servoPin = 25;  

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  myServo.write(0); 
  Serial.println("Type any angle (0-180) in Serial Monitor to move servo.");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();  

    int angle = input.toInt(); 

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to ");
      Serial.print(angle);
      Serial.println("°");
    } else {
      Serial.println("Error: Only values 0-180 are valid.");
    }
  }
}
