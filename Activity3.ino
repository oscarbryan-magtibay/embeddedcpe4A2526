#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 23;

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  myServo.write(90);
  Serial.println("Enter angles (0-180) separated by spaces:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int start = 0;
    while (start < input.length()) {
      int end = input.indexOf(' ', start);
      if (end == -1) end = input.length();

      String token = input.substring(start, end);
      token.trim();

      if (token.length() && token.toInt() >= 0 && token.toInt() <= 180) {
        int angle = token.toInt();
        myServo.write(angle == 0 ? 10 : angle - 10);
        delay(300);
        myServo.write(angle);
        delay(500);
        Serial.printf("Moved to %d°\n", angle);
      } else if (token.length()) {
        Serial.printf("Invalid input: '%s'\n", token.c_str());
      }

      start = end + 1;
    }
  }
}