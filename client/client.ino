#include <SPI.h>
#include <ESP8266WiFi.h>

//#include <DHT.h>;

//#define DHTPIN D8// what pin we're connected to
//#define DHTTYPE DHT22// DHT 22  (AM2302)
//DHT dht(DHTPIN, DHTTYPE);// Initialize DHT sensor for normal 16mhz Arduino
//float hum;
//float temp;

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
 // dht.begin();
}

void loop () {
  //hum = dht.readHumidity();
  //temp = dht.readTemperature();
  //String TEMP = String("temp: ") + temp;
  client.connect(server, 80);   // Connection to the server
  digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  client.println("2\r");  // sends the message to the server
  //client.println(TEMP);
  //String answer = client.readStringUntil('\r');   // receives the answer from the sever
  //Serial.println("from server: " + answer);
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  digitalWrite(ledPin, HIGH);
  delay(2000);                  // client will trigger the communication after two seconds
}
