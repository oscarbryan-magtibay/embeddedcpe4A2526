const int led = 2;        
const int button = 4;      

int ledState = HIGH;      /
int buttonState;          
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;  
const unsigned long debounceDelay = 50;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);    

  digitalWrite(led, ledState);      
}

void loop() {
  int reading = digitalRead(button);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  digitalWrite(led, ledState);
  lastButtonState = reading;  
}
