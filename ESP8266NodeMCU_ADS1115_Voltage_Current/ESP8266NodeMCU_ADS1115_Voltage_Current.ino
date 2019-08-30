#include <Wire.h>
#include <ADS1115.h>

ADS1115 ads;

void setup(void)
{
    Serial.begin(9600);

    // The ADC gain (PGA), Device operating mode, Data rate
    // can be changed via the following functions

    //ads.setGain(GAIN_TWO);          // 2x gain   +/- 2.048V  1 bit = 0.0625mV (default)
    ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 0.1875mV
    // ads.setGain(GAIN_ONE);       // 1x gain   +/- 4.096V  1 bit = 0.125mV
    // ads.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V  1 bit = 0.03125mV
    // ads.setGain(GAIN_EIGHT);     // 8x gain   +/- 0.512V  1 bit = 0.015625mV
    // ads.setGain(GAIN_SIXTEEN);   // 16x gain  +/- 0.256V  1 bit = 0.0078125mV

    ads.setMode(MODE_CONTIN);       // Continuous conversion mode
    // ads.setMode(MODE_SINGLE);    // Power-down single-shot mode (default)

    ads.setRate(RATE_128);          // 128SPS (default)
    // ads.setRate(RATE_8);         // 8SPS
    // ads.setRate(RATE_16);        // 16SPS
    // ads.setRate(RATE_32);        // 32SPS
    // ads.setRate(RATE_64);        // 64SPS
    // ads.setRate(RATE_250);       // 250SPS
    // ads.setRate(RATE_475);       // 475SPS
    // ads.setRate(RATE_860);       // 860SPS

    ads.setOSMode(OSMODE_SINGLE);   // Set to start a single-conversion

    ads.begin();
}

void Irms() {
  uint32_t IrmsPage = millis();
  //while((millis() - IrmsPage) < OLEDSreemHold) {// 安培
    float result;
    float maxValue = 0.50;// store max value here
    float minValue = 4.50;// store min value here
    uint32_t start_time = millis();// millis(millis second)
    while((millis() - start_time) < 1000) {// sample for 1 Sec

      float readAValue = (ads.Measure_SingleEnded(0) * 0.1875/1000);
      if (readAValue > maxValue) {
        maxValue = readAValue;// record the maximum sensor value
      }
      if(readAValue < minValue) {
        minValue = readAValue;// record the maximum sensor value
      }
      /*Serial.println(maxValue);
      Serial.println(minValue);
      Serial.println("---------------");*/
    }
    result = (maxValue - minValue);// Subtract min from max
    //Serial.println(result);
            /*Serial.print("maxValue: ");
        Serial.println(maxValue);
        Serial.print("minValue: ");
        Serial.println(minValue);
        Serial.println("----------------------");*/
    float Irms = (result/2.0) * 0.707;
    float AmpsRMS = (Irms * 1000)/66;
    String SIrms = AmpsRMS + String(" A");
    Serial.println(SIrms);

   //}
}

void Vrms() {
  uint32_t VrmsPage = millis();
    float result = 0.00;
    float maxValue = 0.50;// store max value here
    float minValue = 4.50;// store min value here
    uint32_t start_time = millis();// millis(millis second)
      while((millis()-start_time) < 1000) {//sample for 1 Sec
        float readVValue = ((ads.Measure_SingleEnded(1) * 0.1875 )/1000);
        if (readVValue > maxValue) {
          maxValue = readVValue;// record the maximum sensor value
        }
        if (readVValue < minValue) {
          minValue = readVValue;// record the maximum sensor value
        }
      }
        /*Serial.print("maxValue: ");
        Serial.println(maxValue);
        Serial.print("minValue: ");
        Serial.println(minValue);
        Serial.println("----------------------");*/
      result = (maxValue - minValue);// Subtract min from max
      //Serial.println(result);
      float Vrms = ((result * 0.5 * 500));
      String SVrms = Vrms + String(" V");
      Serial.println(SVrms);
}

void loop(void)
{
    int16_t result01, result03, result13, result23;

    //Serial.println("Getting Differential readings from AIN01, AIN03, AIN13 and AIN23");
    //result01 = ads.Measure_Differential(01);
    //Serial.print("Digital Value of Analog Input between Channel 0 and 1: ");
    //Serial.println(result01);
    /*result03 = ads.Measure_Differential(03);
    Serial.print("Digital Value of Analog Input between Channel 0 and 3: ");
    Serial.println(result03);
    result13 = ads.Measure_Differential(13);
    Serial.print("Digital Value of Analog Input between Channel 1 and 3: ");
    Serial.println(result13);
    result23 = ads.Measure_Differential(23);
    Serial.print("Digital Value of Analog Input between Channel 2 and 3: ");
    Serial.println(result23);

    Serial.println("        ****************************************        ");
    Serial.println(" ");*/
  Irms();
  //Vrms();
    //delay(1000);
}
