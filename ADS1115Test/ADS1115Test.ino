//ADS1115 library
#include <Wire.h>
#include <Adafruit_ADS1015.h>
/*set*/
// set ADS1115
Adafruit_ADS1115 ads(0x48);  /* Use this for the 16-bit version */

void setup() {
  Serial.begin(9600);
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();
}

void loop() {
  float v = (ads.readADC_SingleEnded(0) * 0.1875 / 1000);
  Serial.println(v);
}
 
