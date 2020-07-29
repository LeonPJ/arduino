//ADS1115 library
#include <Wire.h>
#include <Adafruit_ADS1015.h>
//SSD1306 library
#include "SH1106Wire.h"
/*set*/
//set OLED
SH1106Wire display(0x3C, D2, D1);// D2 SDA, D1 SCL
// set ADS1115
Adafruit_ADS1115 ads(0x48);

#define OLEDSreemHold 1500 // 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸

void IrmsVrms() {
  uint32_t RMSPage = millis();
  float Iresult = 0.00;
  float ImaxValue = 0.50;// I max value
  float IminValue = 4.50;// I min value
  float Vresult = 0.00;
  float VmaxValue = 1.00;// V max value
  float VminValue = 3.30;// V min value
  int16_t adc0, adc1;
  int count = 0;
  uint32_t start_time = millis();
    while((millis()-start_time) < 1000) {
      adc0 = ads.readADC_SingleEnded(0);// 電壓
      adc1 = ads.readADC_SingleEnded(1);// 電流
      float readVValue = (adc0 * 0.1875 /1000);
      float readIValue = (adc1 * 0.1875 /1000);
        
      if (readVValue > VmaxValue) { // 電壓n電流大小值判斷
        VmaxValue = readVValue;// record the maximum sensor value
      }
      if (readVValue < VminValue) {
        VminValue = readVValue;// record the maximum sensor value
      }
            
      if (readIValue > ImaxValue) { // 電流判斷
        ImaxValue = readIValue;// record the maximum sensor value
       }
      if (readIValue < IminValue) {
        IminValue = readIValue;// record the maximum sensor value
      }
      count++;
    }
    //Serial.println(count);
    Vresult = (VmaxValue - VminValue);
    Iresult = (ImaxValue - IminValue);
    float Irms = ((Iresult/2) * 0.707 * 1000 / 66);
    float Vrms = ((Vresult / 2) * 0.707 * 172);// 172 294
    String SVrms = Vrms + String(" V");
    String SIrms = Irms + String(" A");
      
    /*uint32_t VrmsPage = micros();
      while((micros() - VrmsPage) < OLEDSreemHold) {// 電壓
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "Vrms:");
        display.drawString(OLEDDataX, OLEDDataY, SVrms);
        display.display();
    }*/
      
      uint32_t IrmsPage = millis();
      while((millis() - IrmsPage) < OLEDSreemHold) {// 電流
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "Irms:");
        display.drawString(OLEDDataX, OLEDDataY, SIrms);
        display.display();
      }
      
      /*uint32_t SPage = millis();
      while((millis() - SPage) < OLEDSreemHold) {// 瓦特  
        float S = Vrms * Irms;
        String SS = S + String(" W");
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "S:");
        display.drawString(OLEDDataX, OLEDDataY, SS);
        display.display();
      }*/
      
      /*uint32_t PFPage = millis();
      while((millis() - PFPage) < OLEDSreemHold) {// 功率因數
        //float PF = (PFValue / (Vrms * Irms));
        //String SPF = PF + String(" ");
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "PF:");
        //display.drawString(OLEDDataX, OLEDDataY, SPF);
        display.display();
      }*/

  }


void setup() {
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);//24 16 10
  display.clear(); // clearing the display
  display.drawString(0, 0, "Initial setting");
  display.display();
  display.setFont(ArialMT_Plain_24);
  Serial.begin(115200);
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();
  pinMode(D5, INPUT);// local control
  pinMode(D3, OUTPUT);// relay control
}

void loop() {
  IrmsVrms();
}
