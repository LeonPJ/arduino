#include "ArduinoJson.h"

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here

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
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    Serial.println("connect");
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin("http://120.126.8.126/api/device_info?api_token=kB9btlaYg4p7Xokqqbk8YyEna4VehQKwcI7DXwToU6nEjJvW5paJ5ZXZCt22");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.POST("device_serial_number=d3ddc848-64d8-49fd-bc5f-c5e1848d5344&parent=0&category=controller&description=SSData&input_distance=5&output_distance=4096&version=1.0.0&status=0&measure_item=%E9%9B%BB%E5%A3%93%7C%E9%9B%BB%E6%B5%81%7C%E7%93%A6%E6%95%B8%7C%E5%8A%9F%E7%8E%87%E5%9B%A0%E6%95%B8%7C%E7%8B%80%E6%85%8B&measure_unit=V%7CA%7CW%7CP.F.%7CSTATUS");  //Send the request
    String JSONMessage = {http.getString()}; //Original message
    StaticJsonBuffer<300> JSONBuffer; //Memory pool
    JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
    if(!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(5000);
      return;
    }
  const char * message = parsed["message"]; // get api key
  Serial.println(message);
  Serial.println();
  delay(10000);
  }
}
//{"code":"000","message":"success","data":{"device":true,"formula":true}}
