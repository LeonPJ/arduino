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
  //pinMode(16, OUTPUT);
  /*pinMode(17, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);*/
  /*pinMode(2, INPUT);
  pinMode(4, INPUT);*/
  pinMode(36, INPUT);// voltage
  pinMode(39, INPUT);// current
}

void loop() {
  /*display.clear(); // clearing the display
  display.drawString(0, 0, "Hello world");
  display.drawString(0, 24, "Hola");
  display.display();*/
  /*delay(1000);
  digitalWrite(16, HIGH);
  delay(1000);
  digitalWrite(16, LOW);*/
  /*delay(1000);
  digitalWrite(17, HIGH);
  delay(1000);
  digitalWrite(17, LOW);
  delay(1000);
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
  digitalWrite(18, HIGH);
  delay(1000);
  digitalWrite(18, LOW);*/
  //delay(100);
  /*Serial.print("B1 status: ");
  Serial.println(digitalRead(2));
  Serial.print("B2 status: ");
  Serial.println(digitalRead(4));*/
  delay(1000);
  Serial.print("voltage: ");
  Serial.println(analogRead(36));
  Serial.print("current: ");
  Serial.println(analogRead(39));
  Serial.println("-----------------");
}
