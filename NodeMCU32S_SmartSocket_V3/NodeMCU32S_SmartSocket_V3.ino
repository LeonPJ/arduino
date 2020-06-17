#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <SH1106Wire.h>

#define IPin 35// I analog pin39
#define VPin 34// V analog pin36

#define SCL 22// OLED SCL pin
#define SDA 21// OLED SDA pin
#define OLEDAddress 0x3C
#define RLED 19// LED pin
#define GLED 18// LED pin
#define YLED 5// LED pin
#define ButtonOne 17// Button pin
#define ButtonTwo 16// Button pin
#define RelayControl 4// Control Relay
#define DataTime 16667// Calculat Data Time
//#define DataTime 1000000// Calculat Data Time
#define OLEDSreemHold 2000// 顯示資料保持1秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 0// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸
#define CurrentCorrectionValuePointFiveToThree 1.22637// 電流Irms在0.5~3 時, 校正參數
#define CurrentCorrectionValueThreeToTwenty 1.1819// 電流Irms在3~20 時, 校正參數
#define VoltagetCorrectionValueFull 0.9649// 電壓Vrms校正參數
#define POffectADC 1.65
const char* ssid = "dlink-7730";
const char* password =  "135791113";
String IP, MAC;

SH1106Wire display(OLEDAddress, SDA, SCL);
float IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount, PmaxValue, PminValue, PreadValue, count, CurrentStatus;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData, CCVIrms, S, Presult, PFCorrectionValue;
uint32_t start_time, FirstPage, SecondPage;
String OLEDI, OLEDV, OLEDRms, OLEDS, OLEDPF;



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
  PmaxValue = -2.7225;// 電壓與電流相乘初始設定為0
  IminValue = 4095;// 電流ADC讀取最大值初始設定為4096
  VminValue = 4095;// 電壓ADC讀取最大值初始設定為4096
  PminValue = 2.7225;
  ITotalData = 0;// 電流n次加總初始設定為0
  VTotalData = 0;// 電壓n次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0
  CCVIrms = 0;//電流校正初始值為0
  //count = 0;
  //-for(DataCount = 0; DataCount < 10; DataCount++){
  start_time = micros();
  while((micros()-start_time) <= DataTime) {// 取樣時間16.6667ms 1s=1000000
    delayMicroseconds(57);// FOR TWO ADC IN 16.667MS
    IreadValue = analogRead(IPin);// 讀取電流ADC
    delayMicroseconds(57);// FOR TWO ADC IN 16.667MS
    VreadValue = analogRead(VPin);// 讀取電壓ADC
    //PreadValue = ((IreadValue*3.3/4095) - POffectADC) * ((VreadValue*3.3/4095) - POffectADC);

    if(IreadValue > ImaxValue) {// 電流判斷大小值
      ImaxValue = IreadValue;
      }
    if(IreadValue < IminValue) {
      IminValue = IreadValue;
    }
      
    if(VreadValue > VmaxValue) {// 電壓判斷大小值
      VmaxValue = VreadValue;
    }
    if(VreadValue < VminValue) {
      VminValue = VreadValue;
    }
    
    /*if(PreadValue > PmaxValue) {// PF判斷大小值
      PmaxValue = PreadValue;
    }
    if(PreadValue < PminValue) {
      PminValue = PreadValue;
    }*/
      //Status();
  }

  Iresult = ((ImaxValue - IminValue) * 3.3)/4095;// 將電流
  Vresult = ((VmaxValue - VminValue) * 3.3)/4095;
  //Presult = (PmaxValue + PminValue);
  //Serial.println(count);
  Irms = ((Iresult/2) * 0.707 * 22.727);// 30A for 66 22.727
  // 電流校整參數
  if(0.5 <= Irms <= 3){// 當電流rms值在0.5~3安培時
    CCVIrms = Irms * CurrentCorrectionValuePointFiveToThree;
    CurrentStatus = 1;
  }else if(3 < Irms <= 20){// 當電流rms值在3~20安培時
    CCVIrms = Irms * CurrentCorrectionValueThreeToTwenty;
    CurrentStatus = 2;
  }else{
     CCVIrms = Irms * 0;
     CurrentStatus = 3;
  }
  Vrms = ((Vresult/2) * 0.707 * 257.13);
  //S = CCVIrms * Vrms;
  if(CurrentStatus == 1){
    PFCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValuePointFiveToThree * 257.13);
  }else if(CurrentStatus == 2){
    PFCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValueThreeToTwenty * 257.13);
  }else if(CurrentStatus == 3){
    PFCorrectionValue = ((Presult/2) * 22.727 * 257.13 * 0);
  }
  //PFCorrectionValue = ((Presult/2)/((Iresult/2*0.707)*(Vresult/2*0.707)));
  OLEDRms = String("rms");
  OLEDI = String("I  :") + CCVIrms + String("A");
  OLEDV = String("V  :") + Vrms + String("V");
  OLEDS = String("S:") + S + String("W");
  OLEDPF = String("P.F.:") + PFCorrectionValue;
  FirstPage = millis();
  while((millis() - FirstPage) < OLEDSreemHold){ //電流電壓
    display.clear(); // clearing the display
    display.setFont(ArialMT_Plain_10);// 設定rms字形大小
    display.drawString(5, 12, OLEDRms);// for I
    display.drawString(12, 36, OLEDRms);// for V
    display.setFont(ArialMT_Plain_24);// 設定顯示量測值字形大小
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV); 
    display.display();
    //Status();
  }
  /*SecondPage = millis();
  while((millis() - SecondPage) < OLEDSreemHold){ //功率功率因數
    display.clear(); // clearing the display
    display.setFont(ArialMT_Plain_24);// 設定顯示量測值字形大小
    display.drawString(OLEDItemX, OLEDItemY, OLEDS);
    display.drawString(OLEDDataX, OLEDDataY, OLEDPF); 
    display.display();
    //Status();
  }*/

}

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  //delay(1000);
  //Serial.println("Connecting to ");
  //Serial.println(ssid);
  //connect to your local wi-fi network
  //WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  //while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "wait");
    display.display();  
    //Serial.print(".");
  //}
  //Serial.println("");
  //Serial.println("WiFi connected");
  //IP = WiFi.localIP().toString();// change to string
  //MAC = WiFi.macAddress();

  pinMode(5, OUTPUT);// LED
  pinMode(18, OUTPUT);// LED
  pinMode(19, OUTPUT);// LED
  pinMode(4, OUTPUT);// 繼電器
  pinMode(17, INPUT_PULLUP);// 按鍵B1
  pinMode(16, INPUT_PULLUP);// 按鍵B2
  pinMode(35, INPUT_PULLUP);// 電流訊號
  pinMode(34, INPUT_PULLUP);// 電壓訊號
  display.setFont(ArialMT_Plain_24);//24 16 8
}

void loop() {
  //delay(500);
  //if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    RMS();
 //}else{
  //Serial.println("Error in WiFi connection");
  //String ERR = String("Wi-Fi Fed Out");
    //display.clear(); // clearing the display
    //display.drawString(OLEDItemX, OLEDItemY, ERR);
    //display.display();   
 //}
  //delay(500);
}
