const int potPin = 12;
const int ledPin = 27;

int potValue = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  potValue = analogRead(potPin);
  int brightness = map(potValue, 0, 4095, 0, 255);
  analogWrite(ledPin, brightness);
  delay(10);
}
