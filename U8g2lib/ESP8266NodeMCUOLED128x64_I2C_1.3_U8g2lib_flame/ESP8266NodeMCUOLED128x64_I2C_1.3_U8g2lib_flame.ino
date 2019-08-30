// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h>

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, D2, D1);

int FlameValue;// flame digital value
#define FlamePin D5// define flame pin D5

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  pinMode(FlamePin, INPUT);// 設定感測器为输入
}

void displayFlame() {
  FlameValue = digitalRead(FlamePin);
  display.setColor(WHITE);
  display.drawString(0, 0, "STATE:");
  if(FlameValue == LOW) {
      display.drawString(0, 32, "FIND FIRE");
    }else{
      display.drawString(0, 32, "NORNAL");
      }
  display.display();
  display.clear();
}

void loop() {
displayFlame();


}
