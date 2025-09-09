#define SERVO_PIN 13
#define SERVO_CHANNEL 0 
#define SERVO_FREQ 50 
#define SERVO_RES 16 

int currentAngle = 90;

void setup() {
  Serial.begin(115200);
  Serial.println("Enter angle (0 - 180):");

  ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);

  writeServo(currentAngle);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    if (input.length() > 0) {
      int angle = input.toInt();
      if (angle >= 0 && angle <= 180) {
        currentAngle = angle;
        writeServo(currentAngle);
        Serial.print("Servo moved to: ");
        Serial.println(currentAngle);
      } else {
        Serial.println("Invalid input! Enter value 0 - 180.");
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
