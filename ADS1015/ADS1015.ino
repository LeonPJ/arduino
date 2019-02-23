#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <MQ2.h>

Adafruit_ADS1115 ads(0x48);// Construct an ads1015 at the default address: 0x48

float Voltage0 = 0.0;
float Voltage1 = 0.0;
float Voltage2 = 0.0;
float Voltage3 = 0.0;
int16_t adc0, adc1, adc2, adc3;  // we read from the ADC, we have a sixteen bit integer as a result


void setup() 
{
  Serial.begin(9600);  
  ads.begin();

}

void loop() 
{

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  
  Voltage0 = (adc0 * 0.1875)/1000;
  Voltage1 = (adc1 * 0.1875)/1000;
  Voltage2 = (adc2 * 0.1875)/1000;
  Voltage3 = (adc3 * 0.1875)/1000;
  
  MQ2 mq2(adc0);
  Serial.print("LPG: ");
  Serial.println(mq2.readLPG());
  /*Serial.print("AIN0: "); 
  Serial.println(Voltage0);
  //Serial.println(ads.readADC_SingleEnded(0));
  
  Serial.print("AIN1: "); 
  Serial.println(Voltage1);
  
  Serial.print("AIN2: "); 
  Serial.println(Voltage2);
  
  Serial.print("AIN3: "); 
  Serial.println(Voltage3);*/
  
  Serial.println("--------------------");
  
  delay(1000);
}

