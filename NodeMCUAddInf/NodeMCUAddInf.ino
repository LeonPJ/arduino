#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

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
   http.begin("http://120.126.8.126/api/device_info?api_token=kB9btlaYg4p7Xokqqbk8YyEna4VehQKwcI7DXwToU6nEjJvW5paJ5ZXZCt22");  //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   int httpCode = http.POST("device_serial_number=3e2c1850-e12b-4e81-85e9-4b280a126ecb&parent=0&category=controller&description=EMData&input_distance=5&output_distance=1024&version=1.0.0&status=0&measure_item=%E4%B8%80%E6%B0%A7%E5%8C%96%E7%A2%B3%7C%E4%BA%8C%E6%B0%A7%E5%8C%96%E7%A2%B3%7C%E6%B6%B2%E5%8C%96%E7%9F%B3%E6%B2%B9%E6%B0%A3%7C%E6%BA%AB%E5%BA%A6%7C%E6%BF%95%E5%BA%A6%7C%E7%81%AB%E7%84%B0&measure_unit=ppm%7Cppm%7Cppm%7CC%7C%25%7CSTATUS");  //Send the request
   String payload = http.getString(); //Get the response payload
   Serial.println(httpCode);  //Print HTTP return code
   Serial.println(payload); //Print request response payload
   http.end();  //Close connection
 }else{
    Serial.println("Error in WiFi connection");   
 }
  delay(10000);  //Send a request every 3 seconds

}
