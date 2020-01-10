#include "ArduinoJson.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here

String IP, MAC, Inf;

String APIKEY = "07aaab96-b507-4afc-9d1a-3175dd653d08";

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {  //check wi-fi is connected to wi-fi network
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();
}
 
void loop() {
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    Serial.println("connect...");
    //delay(15000);
    delay(2000);
    Inf = "http://120.126.8.126/api/switch/" + APIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
    HTTPClient http;// Declare object of class HTTPClient
    //http.begin("http://120.126.8.126/api/switch/07aaab96-b507-4afc-9d1a-3175dd653d08?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");
    http.begin(Inf);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.GET();// Send the request
    //Serial.println(http.getString());
    String JSONMessage = {http.getString()}; // Original message
    String JSONName = JSONMessage;
    String JSONData = JSONMessage;
    String SubstringJSONName = JSONName.substring(43, 49);// get item name
    String SubstringJSONData = JSONData.substring(51, 52);// get item data
    String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
    Serial.println(DeviceStatus);
  }
}
//{"code":"000","message":"success","data":{"status":0}}
