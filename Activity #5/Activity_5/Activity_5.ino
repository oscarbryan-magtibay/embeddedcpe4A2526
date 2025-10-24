#include <Wire.h>
#include <hd44780.h>                       // Main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander class

hd44780_I2Cexp lcd;  // auto-detect I2C backpack

void setup() {
  Wire.begin(21, 22);   // SDA=21, SCL=22 for ESP32

  // initialize LCD with auto size detection
  lcd.begin(16, 2);

  lcd.clear();
  lcd.print("Happy");
  lcd.setCursor(0, 1);
  lcd.print("Birthday!");
}

void loop() {
}
