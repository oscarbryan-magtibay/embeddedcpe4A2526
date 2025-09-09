#include <Servo.h>   // Use the Arduino Servo library

Servo myServo;       
int servoPin = 9;    // Change to the pin where your servo signal wire is connected

void setup() {
  Serial.begin(9600);     // Standard baud rate for Arduino
  myServo.attach(servoPin); 
  Serial.println("Enter angle between 0 and 180:");
}

void loop() {
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();  

    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to: ");
      Serial.print(angle);
      Serial.println(" degrees");
    } else {
      Serial.println("Invalid input! Enter a value from 0 to 180.");
    }
  }
}
