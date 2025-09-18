#include "BluetoothSerial.h"

// === Servo Configuration ===
const int SERVO_PIN      = 13;
const int SERVO_CHANNEL  = 0;
const int SERVO_FREQ     = 50;
const int SERVO_RES      = 16;

// === Pulse Widths ===
const int SERVO_MIN_US   = 500;   // 0°
const int SERVO_MAX_US   = 2400;  // 180°
const int SERVO_PERIOD   = 20000; // 50Hz period in microseconds

BluetoothSerial SerialBT;
int currentAngle = 90;

// --- Function Declarations ---
void initServo();
void writeServo(int angle);
void handleIncomingData();

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_ServoReceiver");
  Serial.println("Receiver ready. Waiting for Master...");

  initServo();
  writeServo(currentAngle);
}

void loop() {
  if (SerialBT.available()) {
    handleIncomingData();
  }
}

// === Functions ===

void initServo() {
  ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
}

void handleIncomingData() {
  String input = SerialBT.readStringUntil('\n');
  input.trim();
  if (input.isEmpty()) return;

  int angle = input.toInt();
  if (angle < 0 || angle > 180) return;

  currentAngle = angle;
  writeServo(currentAngle);
  Serial.printf("Servo moved to: %d\n", currentAngle);
}

void writeServo(int angle) {
  int pulseUs = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  int duty = (pulseUs * 65535) / SERVO_PERIOD;
  ledcWrite(SERVO_CHANNEL, duty);
}
