#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "ArduinoJson.h"

const char* ssid = "dlink-7730";    // Enter SSID here
const char* password = "135791113";  //Enter Password here

String IP, MAC;
String NameDevice = "DoorCount";

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();
}
 
void loop() {
  //if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;    //Declare object of class HTTPClient
    delay(15000);
    String Inf = "name=" + NameDevice + "&mac_address=" + MAC + "&ip_address=" + IP;
    http.begin("http://120.126.8.126/api/device?api_token=kB9btlaYg4p7Xokqqbk8YyEna4VehQKwcI7DXwToU6nEjJvW5paJ5ZXZCt22");      //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.POST(Inf);//Send the request
    String JSONMessage = {http.getString()}; //Original message
    StaticJsonBuffer<300> JSONBuffer; //Memory pool
    JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
    if(!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(5000);
      return;
    }
    const char * data = parsed["data"]; // get api key
    Serial.println(data);
    Serial.println(); 
 /*}else{
    Serial.println("Error in WiFi connection");   
 }*/

}
