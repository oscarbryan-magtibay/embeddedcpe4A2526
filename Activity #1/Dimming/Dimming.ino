const int ledPin = 2;      // LED pin

const int potPin = 34;     // Potentiometer pin (analog input)

int potValue = 0;          // Variable to store potentiometer value

int pwmValue = 0;          // PWM value to control LED brightness


void setup() {

  // Initialize the LED pin as an output

  pinMode(ledPin, OUTPUT);


  // Start serial communication for debugging

  Serial.begin(115200);

}


void loop() {

  // Read the potentiometer value (0 to 4095)

  potValue = analogRead(potPin);


  // Map the potentiometer value to a range of 0-255 for PWM

  pwmValue = map(potValue, 0, 4095, 0, 255);


  // Use analogWrite() to set the LED brightness (PWM control)

  analogWrite(ledPin, pwmValue);


  // Print potentiometer and pwm value for debugging

  Serial.print("Potentiometer Value: ");

  Serial.print(potValue);

  Serial.print(" | PWM Value: ");

  Serial.println(pwmValue);


  // Small delay for smoother transitions

  delay(10);

}