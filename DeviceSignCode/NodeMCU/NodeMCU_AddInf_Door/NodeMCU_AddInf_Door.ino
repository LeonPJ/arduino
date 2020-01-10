#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here

String IP, MAC, Inf;
String APIKEY = "00fab53f-0acc-442f-ba99-14edd2cbc24a";
String DeviceID = "1";
String DeviceDescription = "Count people";

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

 if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
  Serial.println("connect");
  delay(15000);
  Inf = "device_serial_number=" + APIKEY + "&parent=" + DeviceID + "&category=controller&description=" + DeviceDescription + "&input_distance=5&output_distance=4095&version=1.0.0&status=0&measure_item=count&measure_unit=person";
  HTTPClient http;    //Declare object of class HTTPClient
  http.begin("http://120.126.8.126/api/device_info?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");  //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(Inf);  //Send the request
  String JSONMessage = {http.getString()}; //Original message
  String SubstringJSONMessage = JSONMessage.substring(41, 71);
  Serial.println(SubstringJSONMessage);
  //http.end();  //Close connection
 }else{
    Serial.println("Error in WiFi connection");   
 }
  //delay(10000);  //Send a request every 3 seconds

}
