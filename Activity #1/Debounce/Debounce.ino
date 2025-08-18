const int buttonPin = 15;   
const int ledPin = 2;       

int buttonState = LOW;      
int lastButtonState = LOW;  
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;   

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  int reading = digitalRead(buttonPin);

  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      
      if (buttonState == LOW) { // button pressed (since INPUT_PULLUP)
        digitalWrite(ledPin, !digitalRead(ledPin));
      }
    }
  }

  lastButtonState = reading;
}
