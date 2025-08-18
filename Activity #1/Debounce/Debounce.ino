const int led = 2;
const int button = 4;

int ledState = LOW;
int buttonState;
int lastButtonState = HIGH;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP); 
  digitalWrite(led, ledState);
}

void loop() {
  buttonState = digitalRead(button);

  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50); 
    if (digitalRead(button) == LOW) {  
      ledState = !ledState; 
      digitalWrite(led, ledState);
    }
  }

  lastButtonState = buttonState;
}
