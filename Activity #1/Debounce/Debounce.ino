// Debounce LED using ESP32
const int buttonPin = 4;  // Button input pin
const int ledPin = 2;     // LED output pin

int ledState = LOW;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {  // Button pressed
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }

  lastButtonState = reading;
}
