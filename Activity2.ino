const int ledPin = 2;
const int potPin = 32;
void setup() {
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(ledPin, 0);
}
void loop() {
  int potValue = analogRead(potPin);
  int pwmValue = map(potValue, 0, 4095, 0, 255);
  ledcWrite(0, pwmValue);
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" PWM: ");
  Serial.println(pwmValue);
  delay(10);
}

