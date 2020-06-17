// SH1106
#include <Wire.h>
#include <SH1106Wire.h>

#define IPin 35// I analog pin39
#define VPin 34// V analog pin36

#define SCL 22// SH1106 SCL
#define SDA 21// SH1106 SDA
#define OLEDAddress 0x3C
#define ScreenHold 1000// 顯示資料保持 1 秒
#define OLEDItemX 0 // 顯示OLED名稱 X 軸
#define OLEDItemY 0 // 顯示OLED名稱 Y 軸
#define OLEDDataX 0 // 顯示OLED數據 X 軸
#define OLEDDataY 24// 顯示OLED數據 Y 軸

#define SampleTime 16667// ADC取樣時間

int count;
int MaxMinCount;
int IADCArray[121];// 設定矩陣讀取電流ADC 122筆
int VADCArray[121];// 設定矩陣讀取電壓ADC 122筆
int Imin = 4095;// 電流ADC最小 4095
int Imax = 0;   // 電流ADC最大 0
int Vmin = 4095;// 電壓ADC最小 4095
int Vmax = 0;   // 電壓ADC最大 0
String SIrms;
String SVrms;
uint32_t FirstPage, SecondPage, DataTime;

SH1106Wire display(OLEDAddress, SDA, SCL);

int IADC(){// 讀取電流ADC
  return analogRead(IPin);
}

int VADC(){// 讀取電壓ADC
  return analogRead(VPin);
}

float Iv(int IMinData, int IMaxData){// 計算電流電壓值
  return (((IMaxData - IMinData) * 3.3)/4095);
  //Serial.println((((IMaxData - IMinData) * 3.3)/4095));
}

float Vv(int VMinData, int VMaxData){// 計算電壓電壓值
  return (((VMaxData - VMinData) * 3.3)/4095);
}

float Irms(float IrmsData){
  float TempIrms  = ((IrmsData/2) * 0.707 * 22.727);
  if(0.5 <= TempIrms <= 3){       // 當電流 rms 值在 0.5~3 安培時
    return (TempIrms * 1.22637);  // 1.22637 為電流 Irms 在 0.5~3 時校正參數
    //CurrentStatus = 1;
  }else if(3 < TempIrms <= 20){   // 當電流 rms 值在 3~20 安培時
    return (TempIrms * 1.1819);   // 1.1819 為電流 Irms 在3~20 時校正參數
    //CurrentStatus = 2;
  }else{
     return (TempIrms * 0);
     //CurrentStatus = 3;
  }
}

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  count = 0;
  DataTime = micros();
   while((micros()-DataTime) <= SampleTime) {// 取樣時間16.6667ms 1s=1000000
    delayMicroseconds(57);
    //IADCArray[count] = IADC();
    IADCArray[count] = analogRead(IPin);
    //Serial.println(IADCArray[count]);
    delayMicroseconds(57);
    //VADCArray[count] = VADC();
    VADCArray[count] = analogRead(VPin);
    count++;
   }
   //Serial.println(count);
  //count = 0;
  //for(MaxMinCount = 0; MaxMinCount < 121; MaxMinCount++){
  for(int MaxMinCount = 0; MaxMinCount<122; MaxMinCount++){
    // 電流ADC比大小
    if(IADCArray[MaxMinCount] > Imax){
      Imax = IADCArray[MaxMinCount];
    }
    delayMicroseconds(50);   
    if(IADCArray[MaxMinCount] < Imin){
      Imin = IADCArray[MaxMinCount];
    }
    delayMicroseconds(50);   
    // 電壓ADC比大小
    /*if(VADCArray[MaxMinCount] > Vmax){
      Vmax = VADCArray[MaxMinCount];
    }
    if(VADCArray[MaxMinCount] < Vmin){
      Vmin = VADCArray[MaxMinCount];
    }*/
  //}
  //Serial.println(IADCArray[MaxMinCount]);
  }

  float zz = Iv(Imin, Imax);
  //Irms(zz)
  //Serial.println(zz);
  //SIrms = Irms(zz);
  //String SSImin = String(SIrms);
  //FirstPage = micros();
  String SImin = String (Irms(zz));
  FirstPage = millis();
  while((millis() - FirstPage) <= ScreenHold){ //電流
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(OLEDDataX, OLEDDataY, SImin);
    //Serial.println(SSImin);
    display.display();
  }
  //delay(1000);
}
