const int buttonPin = 14;    
const int ledPin = 2;        

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  

int lastButtonState = HIGH;   
int buttonState;
int ledState = LOW;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.begin(115200);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        Serial.println("Button Pressed - LED Toggled");
      }
    }
  }

  lastButtonState = reading;
}