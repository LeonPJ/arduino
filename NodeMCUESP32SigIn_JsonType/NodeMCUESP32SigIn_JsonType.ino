#include "ArduinoJson.h"

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
}
 
void loop() {
  String inf = "name=SmartSocket1&mac_address=" + MAC + "&ip_address=" + IP;
  HTTPClient http;    //Declare object of class HTTPClient
  http.begin("http://120.126.8.126/api/device?api_token=kB9btlaYg4p7Xokqqbk8YyEna4VehQKwcI7DXwToU6nEjJvW5paJ5ZXZCt22");      //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(inf);   //Send the reques
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
  delay(10000);
}

//{"code":"000","message":"success","data":"7f68f217-fd91-4757-bf61-4759866728c5"}
