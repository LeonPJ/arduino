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
  //Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.begin(9600);
  /*WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");*/
  
}

void loop() {
  /*WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  if (request.indexOf("/ON") != -1)  {
   irsend.sendNEC(0x80FFC13E);
  }
  if (request.indexOf("/OFF") != -1)  {
   irsend.sendNEC(0x80FFC13E);
  }*/
  
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
        while(analogRead(A0) != 4){
          delay(100);
          }
            irsend.sendNEC(0x80FFF10E);
        break;

     case 108:// 擺 l
        while(analogRead(A0) != 990){
          delay(100);
          }
            irsend.sendNEC(0x80FFF10E);
        break;

     case 114:// 擺 r
        while(analogRead(A0) != 641){
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

