const int buttonPin = 14;    // Push button pin
const int ledPin = 2;        // LED pin (Onboard LED on many ESP32s)

// Debounce variables
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // milliseconds

int lastButtonState = HIGH;    // Assume button is not pressed initially
int buttonState;
int ledState = LOW;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.begin(115200);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset debounce timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has stabilized
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new state is LOW (button press)
      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        Serial.println("Button Pressed - LED Toggled");
      }
    }
  }

  lastButtonState = reading;
}