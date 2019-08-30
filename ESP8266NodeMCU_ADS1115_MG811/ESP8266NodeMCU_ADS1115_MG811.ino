/*library*/
//ADS1115 library
#include <Wire.h>
#include <ADS1115.h>
ADS1115 ads;
#define MG811_PIN (ads.Measure_SingleEnded(1))
//#define MG_PIN                       (A0)     //define which analog input channel you are going to use
#define BOOL_PIN (2)
#define DC_GAIN (6)   //define the DC gain of amplifier 8.5
#define READ_SAMPLE_INTERVAL (50)    //define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)     //define the time interval(in milisecond) between each samples in normal operation
#define ZERO_POINT_VOLTAGE (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define REACTION_VOLTGAE (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
float CO2Curve[3] = {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   

float MGRead(int mg_pin) {
  int i;
  float v=0;
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    v += (mg_pin * 0.1875 / 1000);
    delay(READ_SAMPLE_INTERVAL);
    }
  v = (v/READ_SAMPLE_TIMES);
  //v = (ads.Measure_SingleEnded(1) * 0.1875 / 1000);
  return v;  
}

int  MGGetPercentage(float volts, float *pcurve) {
  if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
    return -1;
  } else { 
    return (pow(10, (((volts/DC_GAIN)-pcurve[1])/pcurve[2]) + pcurve[0]));
  }
}

void setup() {
  Serial.begin(9600);
  ads.setGain(GAIN_TWOTHIRDS);// 2/3x gain +/- 6.144V  1 bit = 0.1875mV
  ads.setMode(MODE_CONTIN);// Continuous conversion mode
  ads.setRate(RATE_128);// 128SPS (default)
  ads.setOSMode(OSMODE_SINGLE);// Set to start a single-conversion
  ads.begin();
  pinMode(BOOL_PIN, INPUT);//set pin to input
  digitalWrite(BOOL_PIN, HIGH);//turn on pullup resistors    
}
 
void loop() {
  int percentage;
  float volts;
  volts = MGRead(MG811_PIN);
  Serial.print( "SEN-00007:" );
  Serial.print(volts); 
  Serial.print( "V           " );
  percentage = MGGetPercentage(volts,CO2Curve);
  Serial.print("CO2:");
  if (percentage == -1) {
        Serial.print( "<400" );
    } else {
        Serial.print(percentage);
    }
 
    Serial.print( "ppm" );  
    Serial.print("\n");
 
    if (digitalRead(BOOL_PIN) ){
        Serial.print( "=====BOOL is HIGH======" );
    } else {
        Serial.print( "=====BOOL is LOW======" );
    }
 
    Serial.print("\n");
 
    delay(200);
}
