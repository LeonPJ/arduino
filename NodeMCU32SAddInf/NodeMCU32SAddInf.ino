#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here

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
  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println(WiFi.localIP());
  //Serial.println(WiFi.macAddress());
}
 
void loop() {

 if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
  Serial.println("connect");
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://120.126.8.126/api/device_info?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");  //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   int httpCode = http.POST("device_serial_number=8cedb77d-8a73-4083-8a98-54e522df7298&parent=0&category=controller&description=TESTData&input_distance=4095&output_distance=0&version=1.0.0&status=0&measure_item=STATUS&measure_unit=0");  //Send the request
   String payload = http.getString(); //Get the response payload
   Serial.println(httpCode);  //Print HTTP return code
   Serial.println(payload); //Print request response payload
   http.end();  //Close connectionz
 }else{
    Serial.println("Error in WiFi connection");   
 }
  delay(10000);  //Send a request every 3 seconds

}
//{"code":"000","message":"success","data":{"device":true,"formula":true}}
