// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h> //alis for `#include "SH1106Wire.h"`


#include <DHT.h>

#define DHTPIN D6     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 11


// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, D2, D1);

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  dht.begin(); // initialize dht
}

void displayTempHumid() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  display.setColor(WHITE);
  display.drawString(0, 0, "TEMPERATURA");
  display.drawString(0, 26, String(t) + " C");
  display.display();
  display.clear();
  delay(1000);
  display.clear();
  display.drawString(0, 0, "HUMIDITY");
  display.drawString(0, 26, String(h) + " %");
  display.display();
  display.clear();
  delay(1000);
  
}

void loop() {
// clearing the display
displayTempHumid();


}
