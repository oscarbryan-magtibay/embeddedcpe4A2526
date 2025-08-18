const int buttonPin = 15;
const int ledPin = 2;

int buttonState = 0; 
int lastButtonState = 0;
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;

bool ledState = LOW; 

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && lastButtonState == HIGH) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }
  lastButtonState = reading;
}
