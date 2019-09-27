#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "dlink-7730";    // Enter SSID here
const char* password = "135791113";  //Enter Password here

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
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}
 
void loop() {

 if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
   
   HTTPClient http;    //Declare object of class HTTPClient

   http.begin("http://120.126.8.126/api/device?api_token=kB9btlaYg4p7Xokqqbk8YyEna4VehQKwcI7DXwToU6nEjJvW5paJ5ZXZCt22");      //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   int httpCode = http.POST("name=EnvironmentalMonitoring&mac_address=BC%3ADD%3AC2%3A17%3A64%3A58%0A&ip_address=192.168.0.104%0A");   //Send the request
   String payload = http.getString();                  //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(30000);  //Send a request every 30 seconds

}
