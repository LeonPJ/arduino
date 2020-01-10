#include "ArduinoJson.h"

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here

String IP, MAC, Inf;

String APIKEY = "07aaab96-b507-4afc-9d1a-3175dd653d08";
String DeviceID = "1";
String DeviceDescription = "Smart Socket";
//String DeviceDescription = "Test Device";

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();
}
 
void loop() {
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    Serial.println("connect");
    delay(15000);
    Inf = "device_serial_number=" + APIKEY + "%0A&parent=" + DeviceID + "&category=controller&description=" + DeviceDescription + "&input_distance=5&output_distance=4095&version=1.0.0&status=0&measure_item=%E9%9B%BB%E5%A3%93%7C%E9%9B%BB%E6%B5%81%7C%E5%8A%9F%E7%8E%87%7C%E5%8A%9F%E7%8E%87%E5%9B%A0%E6%95%B8&measure_unit=V%7CA%7CW%7CP.F.%0A";
    
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin("http://120.126.8.126/api/device_info?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.POST(Inf);  //Send the request
    String JSONMessage = {http.getString()}; //Original message
    
    StaticJsonBuffer<300> JSONBuffer; //Memory pool
    JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
    if(!parsed.success()) {//Check for errors in parsing
      Serial.println("Parsing failed");
      delay(5000);
      return;
    }
    const char * message = parsed["message"];
    Serial.println(message);

  }
}
//{"code":"000","message":"success","data":{"device":true,"formula":true}}
