#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h> //alis for `#include "SH1106Wire.h"`

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, D2, D1);

static const unsigned char PROGMEM str_1[] =  
{   
0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x3F,0xF8,0x21,0x08,0x21,0x08,0x21,0x08,
0x21,0x08,0x21,0x08,0x3F,0xF8,0x21,0x08,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
  };//中

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  display.clear(); // clearing the display
  display.drawString(10, 0, "Hello World!!");
  display.drawString(20, 20, "Hello World!!");
  display.drawString(30, 30, "Hello World!!");
  display.display();
}
