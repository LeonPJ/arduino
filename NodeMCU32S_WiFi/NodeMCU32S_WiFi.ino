#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <SH1106Wire.h>

#define SCL 22// OLED SCL pin
#define SDA 21// OLED SDA pin
#define OLEDAddress 0x3C

#define OLEDSreemHold 1000// 顯示資料保持1秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 0// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸

SH1106Wire display(OLEDAddress, SDA, SCL);
 
const char* ssid = "dlink-7730";
const char* password =  "135791113";

String IP, MAC;

int i = 0;;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();
  //Serial.println(IP);
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}
 
void loop() {
 i++;
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  
    
    String ii = String(i);// Count
    display.clear(); // clearing the display
    display.drawString(0, 0, IP); 
    display.drawString(0, 48, ii);
    display.display();
  
 }else{
    Serial.println("Error in WiFi connection");   
 }
  delay(500);  //Send a request every 30 seconds
}
