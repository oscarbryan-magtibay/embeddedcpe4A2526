#define builtInLed 2

void steup() {
  pinMode(builtInLed, OUTPUT);
}

void loop() {
  digitalWrite(builtInLed, HIGH);
  delay(1000);
  digitalWrite(builtInLed, LOW);
  delay(1000);
}