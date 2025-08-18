#define buttonPin  14    // GPIO pin for button
#define ledPin     2     // GPIO pin for LED

unsigned long lastDebounceTime = 0;  // Time the button was last toggled
unsigned long debounceDelay = 50;   // Debounce delay in milliseconds

int lastButtonState = HIGH;          // previous reading
int buttonState = HIGH;              // debounced button state

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // use internal pull-up resistor
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // reset debounce timer
  }

  // Only change the button state if the reading stayed the same for debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Update button state if it's changed
    if (reading != buttonState) {
      buttonState = reading;

      // If button is pressed (active LOW), toggle LED
      if (buttonState == LOW) {
        digitalWrite(ledPin, !digitalRead(ledPin));  // toggle LED
      }
    }
  }

  lastButtonState = reading;
}
