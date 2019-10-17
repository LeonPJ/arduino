#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h> //alis for `#include "SH1106Wire.h"`

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, 21, 22); // SDA SCL

void setup() {
  //Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  pinMode(5, OUTPUT);// LED
  pinMode(8, OUTPUT);// LED
  pinMode(16, OUTPUT);// 繼電器
  pinMode(17, OUTPUT);// LED
  pinMode(2, INPUT_PULLUP);// 按鍵
  pinMode(4, INPUT_PULLUP);// 按鍵
  pinMode(14, INPUT_PULLUP);// 電流訊號
  pinMode(13, INPUT_PULLUP);// 電壓訊號
}

void loop() {
  delay(1000);
  digitalWrite(16, HIGH);
  display.clear(); // clearing the display
  display.drawString(0, 0, "1");
  display.display();
  delay(1000);
  digitalWrite(16, LOW);
  display.clear(); // clearing the display
  display.drawString(0, 0, "2");
  display.display();
}
