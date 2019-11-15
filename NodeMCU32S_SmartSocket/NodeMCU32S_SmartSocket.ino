#include <Wire.h>
#include <SH1106Wire.h>

#define IPin 14// I analog pin
#define VPin 13// V analog pin
#define SCL 22// OLED SCL pin
#define SDA 21// OLED SDA pin
#define OLEDAddress 0x3C
#define RLED 17// LED pin
#define GLED 5// LED pin
#define YLED 18// LED pin
#define ButtonOne 2// Button pin
#define ButtonTwo 15// Button pin
#define RelayControl 16// Control Relay
#define DataTime 16667// Calculat Data Time

#define OLEDSreemHold 1000// 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸


SH1106Wire display(OLEDAddress, SDA, SCL);
//int count;
int IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData;
uint32_t start_time, FirstPage;
String OLEDI, OLEDV;

float RMS() {
  /*float Iresult;
  float Vresult;

  int IreadValue;
  int VreadValue;

  int ImaxValue = 0;
  int VmaxValue = 0;

  int IminValue = 4096;
  int VminValue = 4096;*/
  ImaxValue = 0;// 電流ADC讀取最大值初始設定為0
  VmaxValue = 0;// 電壓ADC讀取最大值初始設定為0
  IminValue = 4096;// 電流ADC讀取最大值初始設定為4096
  VminValue = 4096;// 電壓ADC讀取最大值初始設定為4096
  
  ITotalData = 0;// 電流10次加總初始設定為0
  VTotalData = 0;// 電壓10次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0

  
  //count = 0;
  //uint32_t start_time = micros();

  for(DataCount = 0; DataCount == 10; DataCount++){
      start_time = micros();
  while((micros()-start_time) < DataTime) {// 取樣時間16.6667ms
    //count ++;
    delayMicroseconds(144);// 取樣次數100次
    IreadValue = analogRead(IPin);
    //delayMicroseconds(1000);
    VreadValue = analogRead(VPin);
    //delayMicroseconds(1000);

    if (IreadValue > ImaxValue) {// 電流判斷大小值
      ImaxValue = IreadValue;
    }
    if(IreadValue < IminValue) {
      IminValue = IreadValue;
    }
    if (VreadValue > VmaxValue) {// 電壓判斷大小值
      VmaxValue = VreadValue;
    }
    if(VreadValue < VminValue) {
      VminValue = VreadValue;
    }
    
  }

  Iresult = ((ImaxValue - IminValue) * 3.3)/4096;
  Vresult = ((VmaxValue - VminValue) * 3.3)/4096;
  Irms = ((Iresult/2) * 0.707 * 22.727);// 20A for 100; 30A for 66 
  Vrms = ((Vresult/2) * 0.707 * 257.13);
  ITotalData += Irms;
  VTotalData += Vrms;
  }
  IAvgData = (ITotalData/10);
  VAvgData = (VTotalData/10);
  
  OLEDI = String("Irms: ") + IAvgData + String(" A"); 
  OLEDV = String("Vrms: ") + VAvgData + String(" V");
  /*Serial.println(OLEDI);
  Serial.println(OLEDV);
  Serial.println("--------");*/
  
  //uint32_t FirstPage = millis();
  FirstPage = millis();
  while((millis() - FirstPage) < OLEDSreemHold) { //電流
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV); 
    display.display();
  }
    //delay(1500);
  //return count;

}


void setup() {
  //Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  pinMode(5, OUTPUT);// LED
  pinMode(8, OUTPUT);// LED
  pinMode(16, OUTPUT);// 繼電器
  pinMode(17, OUTPUT);// LED
  pinMode(2, INPUT_PULLUP);// 按鍵
  pinMode(15, INPUT_PULLUP);// 按鍵
  pinMode(14, INPUT_PULLUP);// 電流訊號
  pinMode(13, INPUT_PULLUP);// 電壓訊號
}

void loop() {
  RMS();
  //int C = RMS();
  //String CC = String(C);
  //digitalWrite(16, LOW);

    /*display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, CC);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV); 
    display.display();*/

}
