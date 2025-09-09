#include <ESP32Servo.h>

Servo myServo;
int servoPin = 23;

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  myServo.write(90);
  Serial.println("Type an angle (0–180) in Serial Monitor.");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); 
    if (input.length() > 0 && input.toInt() == 0 && input != "0") {
      Serial.println("Error: Invalid input. Enter a number 0–180.");
      return;
    }

    int angle = input.toInt();
    if (angle >= 0 && angle <= 180) {
      int resetPos = (angle == 0) ? 10 : angle - 10;
      myServo.write(resetPos);
      delay(300);
      myServo.write(angle);
      Serial.print("Servo moved to ");
      Serial.print(angle);
      Serial.println("°.");
    } else {
      Serial.println("Error: Only values 0–180 are valid.");
    }
  }
}
