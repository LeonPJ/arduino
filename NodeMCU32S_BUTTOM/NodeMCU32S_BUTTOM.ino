#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h> //alis for `#include "SH1106Wire.h"`

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, 21, 22); // SDA SCL

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  pinMode(17, INPUT);
  pinMode(16, INPUT);
}

void loop() {
  int BOneStatus = digitalRead(17);
  int BTwoStatus = digitalRead(16);
  String SBOneStatus = String(BOneStatus);
  String SBTwoStatus = String(BTwoStatus);
  display.clear(); // clearing the display
  display.drawString(0, 0, SBOneStatus);
  display.drawString(0, 24, SBTwoStatus);
  display.display();
  Serial.print("B1: ");
  Serial.println(SBOneStatus);
  Serial.print("B2: ");
  Serial.println(SBTwoStatus);
  Serial.println("-------------------------");
  
  /*Serial.print("B1 status: ");
  Serial.println(digitalRead(15));
  Serial.print("B2 status: ");
  Serial.println(digitalRead(4));
  Serial.println("-----------------");*/
}
