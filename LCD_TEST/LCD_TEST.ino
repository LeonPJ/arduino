#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup(void) 
{  // initializar Serial/ADS/LCD 
  Serial.begin(9600);  
  lcd.begin();
  //lcd.backlight(); 
}

void loop(void) 
{
  lcd.setCursor(0,0);
  lcd.print("HELLO WORLD"); 
  //lcd.setCursor(0, 1);
  //lcd.print(Voltage, 4); // Variable a imprimir.
}

