const int LED_PIN = 16;      
const int POTENTIOMETER_PIN = 34;  
int sensorValue = 0;        
int brightnessLevel = 0;    

void setup() {
  pinMode(LED_PIN, OUTPUT);       
  Serial.begin(115200);           
}

void loop() {
  // Read the analog signal from the potentiometer
  sensorValue = analogRead(POTENTIOMETER_PIN);

  // Convert the 12-bit ADC value (0–4095) to an 8-bit PWM range (0–255)
  brightnessLevel = map(sensorValue, 0, 4095, 0, 255);

  // Adjust LED brightness based on potentiometer input
  analogWrite(LED_PIN, brightnessLevel);

  // Display readings on Serial Monitor
  Serial.print("Analog Input: ");
  Serial.print(sensorValue);
  Serial.print(" | LED Brightness (PWM): ");
  Serial.println(brightnessLevel);

  delay(10);  
}
