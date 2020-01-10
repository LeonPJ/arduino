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

#define OLEDSreemHold 1000// 顯示資料保持1秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 0// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸


SH1106Wire display(OLEDAddress, SDA, SCL);
//int count;
int IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData;
uint32_t start_time, FirstPage;
String OLEDI, OLEDV;
//float a[800] = {0};

int Status(){
  if(digitalRead(ButtonTwo) == HIGH){
    digitalWrite(RelayControl, HIGH);
  }else{
    digitalWrite(RelayControl, LOW);
  }
}

float RMS() {
  ImaxValue = 0;// 電流ADC讀取最大值初始設定為0
  VmaxValue = 0;// 電壓ADC讀取最大值初始設定為0
  IminValue = 4096;// 電流ADC讀取最大值初始設定為4096
  VminValue = 4096;// 電壓ADC讀取最大值初始設定為4096
  
  ITotalData = 0;// 電流n次加總初始設定為0
  VTotalData = 0;// 電壓n次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0

  //count = 0;

  for(DataCount = 0; DataCount < 5; DataCount++){
    start_time = micros();
    while((micros()-start_time) <= DataTime) {// 取樣時間16.6667ms

      delayMicroseconds(144);// 取樣次數100次 144
      IreadValue = analogRead(IPin);
      VreadValue = analogRead(VPin);
      
      /*Iresult = ((IreadValue) * 3.3)/4096;
      a[count] = Iresult;
      count++;*/
      
     if (IreadValue > ImaxValue) {// 電流判斷大小值
        ImaxValue = IreadValue;
      }else if(IreadValue < IminValue) {
        IminValue = IreadValue;
      }
      
      if (VreadValue > VmaxValue) {// 電壓判斷大小值
        VmaxValue = VreadValue;
      }else if(VreadValue < VminValue) {
        VminValue = VreadValue;
      }
      //Status();
      /*Iresult = ((IreadValue) * 3.3)/4096;
      a[count] = Iresult;
      count++;*/
      
    }
    //Serial.println(count);
    /*count = 0;
    for(int i=0; i<305; i++){
      Serial.println(a[count]);
      count++;
    }*/
    /*for(int i=1; i<700; i++){
      Iresult = ((analogRead(IPin)) * 3.3)/4096;
      a[count] = Iresult;
      count++;
    }*/

    /*float MAX, MIN;
    count = 0;
    for(int j=1; j<700; j++){
      if(a[count] > MAX){
        MAX = a[count];
      }
      if(a[count] < MIN){
        MIN = a[count];
      }
      count ++;
    }*/
    
    /*Serial.println(MAX);
    Serial.println(MIN);*/
    //Serial.println("--");
    
    Iresult = ((ImaxValue - IminValue) * 3.3)/4095;
    Vresult = ((VmaxValue - VminValue) * 3.3)/4095;
    
    /*Iresult = ((ImaxValue - IminValue) * 3.2 )/3850 + 0.1;
    Vresult = ((VmaxValue - VminValue) * 3.2 )/3850 + 0.1;*/
    //Serial.println(Iresult);
    
    Irms = ((Iresult/2) * 0.707 * 22.727);// 20A for 100; 30A for 66 
    Vrms = ((Vresult/2) * 0.707 * 257.13);
    //Serial.println(Irms);
    ITotalData += Irms;
    VTotalData += Vrms;
    //count++;
  }
  IAvgData = (ITotalData/5);
  VAvgData = (VTotalData/5);
  //Serial.println(count);
  //OLEDI = String("Irms: ") + Irms + String(" A"); 
  //OLEDV = String("Vrms: ") + Vrms + String(" V");

  OLEDI = String("Irms: ") + IAvgData + String(" A"); 
  OLEDV = String("Vrms: ") + VAvgData + String(" V");
  
  FirstPage = millis();
  while((millis() - FirstPage) < OLEDSreemHold) { //電流
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV); 
    display.display();
    //Status();
  }

  /*for(int i = 0; i<100; i++){
    Serial.println(a[i]);
  }
  Serial.println("----------");*/
  
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

  /*digitalWrite(RelayControl, LOW);
  //delayMicroseconds(150000);
  delay(1000);
  digitalWrite(RelayControl, HIGH);
  delay(30000);*/
  //delay(1000);
  //Status();
  RMS();
  //Serial.println(analogRead(IPin));
  //delay(1000);
}
