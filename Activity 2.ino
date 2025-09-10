const int ledPin = 2;       // LED pin (must be PWM capable)
const int potPin = 34;      // Potentiometer pin (ADC input)

// Variables
int potValue = 0;           
int pwmValue = 0;           

// LEDC (PWM) setup
const int freq = 5000;      // PWM frequency (Hz)
const int ledChannel = 0;   // Use channel 0 (0-15 available)
const int resolution = 8;   // Resolution (8 bits: 0-255)

void setup() {
  // Configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);

  // Attach channel to LED pin
  ledcAttachPin(ledPin, ledChannel);

  // Start serial communication
  Serial.begin(115200);
}

void loop() {
  // Read potentiometer value (0–4095 on ESP32)
  potValue = analogRead(potPin);

  // Map potentiometer value to PWM range (0–255 for 8-bit resolution)
  pwmValue = map(potValue, 0, 4095, 0, 255);

  // Set duty cycle (LED brightness)
  ledcWrite(ledChannel, pwmValue);

  // Debugging output
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | PWM Value: ");
  Serial.println(pwmValue);

  delay(10);
}
