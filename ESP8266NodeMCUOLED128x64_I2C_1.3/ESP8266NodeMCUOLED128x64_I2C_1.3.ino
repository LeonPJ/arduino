#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h> //alis for `#include "SH1106Wire.h"`

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, 21, 22);

void setup() {
  //Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  display.clear(); // clearing the display
  display.drawString(0, 0, "Hello");
  display.drawString(0, 24, "Hello2");
  //display.drawString(30, 30, "Hello World!!");
  display.display();
}
