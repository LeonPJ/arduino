#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>

const uint16_t kIrLed = 16;  // ESP8266 GPIO pin to use. D0
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

const char* ssid = "Green-7730";
const char* password = "135791113";

WiFiServer server(80);
int i = 0;
void setup() {
  irsend.begin();
  Serial.begin(9600);
  
}

void loop() {
  Serial.println(analogRead(A0));
  char order;
  while(Serial.available()) {
    order = Serial.read();
    switch(order){
      case 79:// 開/關 O
        irsend.sendNEC(0x80FFC13E);
        Serial.println("on/off");
        break;

      case 84:// 定時 T
        irsend.sendNEC(0x80FFD12E);
        Serial.println("set time");
        break;

      case 70:// 風速 F
        irsend.sendNEC(0x80FFE11E);
        Serial.println("fan");
        break;

     case 77:// 擺頭 M
        irsend.sendNEC(0x80FFF10E);
        Serial.println("move");
        break;

    case 109:// 擺頭中 m
        while(analogRead(A0) != 797){
          delay(100);
          }
            irsend.sendNEC(0x80FFF10E);
        break;

     case 108:// 擺 l
        while(analogRead(A0) != 886){
          delay(100);
          }
            irsend.sendNEC(0x80FFF10E);
        break;

     case 114:// 擺 r
        while(analogRead(A0) != 696){
          delay(100);
          Serial.println(analogRead(A0));
          }
            irsend.sendNEC(0x80FFF10E);
        break;
    
     case 10://new line ASCII
        break;

     case 13://carriage return ASCII
        break;
        
     default:
        Serial.println("錯誤輸入");
        break;
    }
  }
    
}

