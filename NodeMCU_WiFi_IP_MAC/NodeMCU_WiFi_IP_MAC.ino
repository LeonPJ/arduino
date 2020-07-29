#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

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
  Serial.println(IP);
  Serial.println(MAC);
}
 
void loop() {

}
