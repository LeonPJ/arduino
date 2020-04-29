#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "dlink-7730";               // SSID of your home WiFi
char pass[] = "135791113";               // password of your home WiFi
WiFiServer server(80);                    

IPAddress ip(192, 168, 0, 80);            // IP address of the server
IPAddress gateway(192,168,0,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

void setup() {
  Serial.begin(9600);                   // only for debug
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
  Serial.print(WiFi.localIP());
}

void loop () {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      if(request == "1"){
        client.println("I find u 1.\r");
        
      }else if(request == "2"){
        client.println("I find u 2.\r");
      }
      //Serial.print("From client: ");
      //Serial.println("we get: " + request);
      client.flush();
      //client.println("Hi client .\r"); // sends the answer to the client
    }
    client.stop();                // tarminates the connection with the client
  }
  /*
   String request = client.readStringUntil('\r');
    if (request.indexOf("/LED=OFF") != -1)  {

  }*/
}

