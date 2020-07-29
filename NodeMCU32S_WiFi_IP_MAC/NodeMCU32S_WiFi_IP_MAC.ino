#include <WiFi.h>
#include <HTTPClient.h>
 
const char* ssid = "dlink-7730";
const char* password =  "135791113";

String IP, MAC;

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
  Serial.println(IP);
  Serial.println(MAC);
}
 
void loop() {
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

 }else{
    Serial.println("Error in WiFi connection");   
 }
  delay(500);  //Send a request every 30 seconds
}
