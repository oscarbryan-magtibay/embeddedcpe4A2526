const int ledPin = 2, buttonPin = 4;
int ledState = HIGH, buttonState, lastButtonState = LOW;
unsigned long debounceTime = 50, lastDebounce = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin, ledState);
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) lastDebounce = millis();
  if (millis() - lastDebounce > debounceTime && reading != buttonState) {
    buttonState = reading;
    if (buttonState == HIGH) ledState = !ledState;
  }
  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
}
