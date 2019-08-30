//ADS1115 library
#include <Wire.h>
#include <Adafruit_ADS1015.h>
//SSD1306 library
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
//----------------------------------------------------------------------------------------------------------------------------------------------
/*set*/
// set ADS1115
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//set OLED
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE,D1,D2);

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_ncenR24_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

float IrmsVrms() {
  uint32_t RMSPage = millis();
  //while((millis() - RMSPage) < OLEDSreemHold) {// OELD電壓n電流RMS
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
      //String SVrms = Vrms + String(" V");
      //String SIrms = Irms + String(" A");
      /*char CSVrms[5];
      dtostrf(Vrms, 1, 5, CSVrms);
      u8g2.drawStr(0, 0, "123456789");
      Serial.println(CSVrms);*/
      return Vrms;
      //Serial.println(SIrms);
   //}
  }


void setup() {
  Serial.begin(9600);
  pinMode(D5, INPUT);// local control
  pinMode(D3, OUTPUT);// relay control
  pinMode(D0, OUTPUT);
  //u8g2.begin();
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();

}

void loop() {

    /*digitalWrite(D3, LOW);
  int ButtomControlState = digitalRead(D5);
  // 接合0 打開1
  if(ButtomControlState == HIGH){
    digitalWrite(D3, LOW);// buttom off
    }else if(ButtomControlState == LOW){
      digitalWrite(D3, HIGH);// buttom on
      }else{
        
      }*/

  float VVrms = IrmsVrms();
  char CSVrms[5];
  dtostrf(VVrms, 1, 5, CSVrms);
  Serial.println(CSVrms);
    /*u8g2.firstPage();  
  do {
    //draw();
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  u8g2.drawStr(0,32 , CSVrms);
  } while( u8g2.nextPage() );*/
}
