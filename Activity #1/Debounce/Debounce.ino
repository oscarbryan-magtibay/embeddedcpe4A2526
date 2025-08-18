const int buttonPin = 0;  // the number of the pushbutton pin (GPIO 0)
const int ledPin = 2;     // the number of the LED pin (GPIO 2)

// a
int ledState = HIGH;        // the current state of the LED pin
int buttonState;            // the current reading from the button pin
int lastButtonState = LOW;  // the previous reading from the button pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up resistor for button
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);
}

void loop() {
  // read the state of the button into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if the button has just been pressed
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the reading is stable (debounced), take it as the current state:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the button state is LOW (button press)
      if (buttonState == LOW) {
        ledState = !ledState;  // toggle LED state
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading for the next loop:
  lastButtonState = reading;
}
