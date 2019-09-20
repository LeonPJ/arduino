#include <Wire.h>
//SSD1306 library
#include "SH1106Wire.h"

//----------------------------------------------------------------------------------------------------------------------------------------------
/*set*/
//set OLED
SH1106Wire display(0x3C, 21, 22);// D2 SDA, D1 SCL
#define DELAYTIME 4000// 4組ADC 2000  2組DC 4000
/*
float IrmsVrms() {
  uint32_t RMSPage = millis();
    float Iresult = 0.00;
    float ImaxValue = 0.50;// I max value
    float IminValue = 4.50;// I min value
    float Vresult = 0.00;
    float VmaxValue = 1.00;// V max value
    float VminValue = 3.30;// V min value
    uint32_t start_time = millis();
      while((millis()-start_time) < 1000) {// 電壓量測1秒
        float readVValue = (ads.readADC_SingleEnded(0) * 0.1875 /1000);// 電壓
        float readIValue = (ads.readADC_SingleEnded(1) * 0.1875 /1000);// 電流
        //Serial.println(Wire.requestFrom(0x8000, 15));
        if (readVValue > VmaxValue) { // 電壓n電流大小值判斷
          VmaxValue = readVValue;// record the maximum sensor value
            }else if (readVValue < VminValue) {
              VminValue = readVValue;// record the maximum sensor value
            }
        if (readIValue > ImaxValue) { // 電流判斷
          ImaxValue = readIValue;// record the maximum sensor value
            }else if (readIValue < IminValue) {
              IminValue = readIValue;// record the maximum sensor value
            }
      }
      Vresult = (VmaxValue - VminValue);
      Iresult = (ImaxValue - IminValue);
      //Serial.println(Vresult);
      float Vrms = ((Vresult / 2) * 0.707 * 172);
      float Irms = ((Iresult/2) * 0.707 * 1000 / 66);
      return Vresult;
  }
*/

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);//24 16 10 
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  int Acount = 0;
  int Bcount = 0;
  //int Ccount = 0;
  //int Dcount = 0;
      uint32_t start_time = millis();
      while((millis() - start_time) < 1000) {// 電壓量測1秒
      delayMicroseconds(DELAYTIME);  
      float aValue = analogRead(36);
      Acount++;
      delayMicroseconds(DELAYTIME);  
      float bValue = analogRead(39);
      Bcount++;
      /*delayMicroseconds(DELAYTIME);  
      float cValue = analogRead(34);
      Ccount++;*/
      /*delayMicroseconds(DELAYTIME);  
      float dValue = analogRead(32);
      Dcount++;*/
      }
  String SAcount = String(Acount);
  String SBcount = String(Bcount);
  //String SCcount = String(Ccount);
  //String SDcount = String(Dcount);
  
  display.clear(); // clearing the display
  display.drawString(0, 0, SAcount);
  display.drawString(0, 15, SBcount);
  //display.drawString(0, 30, SCcount);
  //display.drawString(0, 45, SDcount);
  display.display();
  display.clear(); // clearing the display
}
