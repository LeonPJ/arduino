#include <SPI.h>
#include <ESP8266WiFi.h>

byte ledPin = 2;
char ssid[] = "dlink-7730";// SSID of your home WiFi
char pass[] = "135791113";// password of your home WiFi

unsigned long askTimer = 0;

IPAddress server(192,168,0,80);// the fix IP address of the server
WiFiClient client;

void setup() {
  Serial.begin(9600);// only for debug
  WiFi.begin(ssid, pass);// connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(WiFi.localIP());
}

void loop () {
  client.connect(server, 80);   // Connection to the server
  digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  // send
  float L1 = analogRead(A0);
  client.println(L1);  // sends the message to the server
  // receive
  String answer = client.readStringUntil('\r');   // receives the answer from the sever
  //Serial.println("from server: " + answer);
  //String request = client.readStringUntil('\r');
  Serial.println(answer);
  client.flush();
  //digitalWrite(ledPin, HIGH);
  //delay(2000);                  // client will trigger the communication after two seconds
}
