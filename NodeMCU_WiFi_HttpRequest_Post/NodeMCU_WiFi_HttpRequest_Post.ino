#include <ESP8266WiFi.h>
String apiKey = "7U1B3P66P0UIGULC";     //  Enter your Write API key here
const char *ssid =  "sunday";     // Enter your WiFi Name
const char *pass =  "16161616"; // Enter your WiFi Password
const char* server = "api.thingspeak.com";

WiFiClient client;
void setup() 
{
       Serial.begin(115200); 
       delay(10);
       Serial.println("Connecting to ");
       Serial.println(ssid);
       WiFi.begin(ssid, pass);
       while (WiFi.status() != WL_CONNECTED) 
     {
            delay(550);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 }
void loop() {
  if(client.connect(server,80)) {  
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String("10");
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("Send to Thingspeak.");
  }
  client.stop();
  delay(15000);
}
