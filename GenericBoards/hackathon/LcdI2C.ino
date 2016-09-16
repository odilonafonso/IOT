/*
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 * DFRobot.com
 *Compatible with the Arduino IDE 1.0
 *Library version:1.1
 */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setupLCD()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.print("Fala Mirinha !!!");
}

