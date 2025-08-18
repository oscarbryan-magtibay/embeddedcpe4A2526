#include <ESP32Servo.h> 
const int potPin = 34;  
const int servoPin = 13; 

Servo myServo; 

void setup() {
  
  myServo.attach(servoPin);
  
  
  Serial.begin(115200);
}

void loop() {
  int potValue = analogRead(potPin);

  
  int servoAngle = map(potValue, 0, 4095, 0, 180);

  
  myServo.write(servoAngle);
  
 
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | Servo Angle: ");
  Serial.println(servoAngle);

  delay(15); 
}
