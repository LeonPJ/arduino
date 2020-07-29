#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <SH1106Wire.h>

#include "ArduinoJson.h"

#define IPin 35// I analog pin39
#define VPin 34// V analog pin36
#define SRPin 33// SR-501 pin33

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
const char* password = "135791113";
String IP, MAC;

String APIKEY = "5d129a99-2533-47bd-acca-71c22d4733d3";

SH1106Wire display(OLEDAddress, SDA, SCL);
float IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount,  count, CurrentStatus;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData, CCVIrms, S, Presult, PCorrectionValue, PF;
uint32_t start_time, FirstPage, SecondPage;
String OLEDI, OLEDV, OLEDRms, OLEDS, OLEDPF, Inf, ControlTypeOLED, RelayStausOLED;
int SRStatus, AfterSRStatus;

double PmaxValue, PminValue, PreadValue;

float PreadValueForI, PreadValueForV;

float PreadValueForImax, PreadValueForImin, PreadValueForVmax, PreadValueForVmin;



float PageOne(float CCVIrms, float Vrms){// 電流 電壓 顯示 副程式
  OLEDRms = String("rms");
  OLEDI = String("I  :") + CCVIrms + String("A");
  OLEDV = String("V  :") + Vrms + String("V");
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
}

float PageTwo(float W, float PPF){// 消耗功率 功率因數 顯示 副程式
  OLEDS = String("P  :") + W;
  OLEDPF = String("PF :") + PPF;
  SecondPage = millis();
  while((millis() - SecondPage) < OLEDSreemHold){ //功率功率因數
    display.clear(); // clearing the display
    display.setFont(ArialMT_Plain_24);// 設定顯示量測值字形大小
    display.drawString(OLEDItemX, OLEDItemY, OLEDS);
    display.drawString(OLEDDataX, OLEDDataY, OLEDPF); 
    display.display();
    //Status();
  }
}

float RMS() {
  ImaxValue = 0;// 電流ADC讀取最大值初始設定為0
  VmaxValue = 0;// 電壓ADC讀取最大值初始設定為0
  PmaxValue = -0.936;// 電壓與電流相乘初始設定為0
  IminValue = 4095;// 電流ADC讀取最小值初始設定為4096
  VminValue = 4095;// 電壓ADC讀取最小值初始設定為4096
  PminValue = 0.936; 
  ITotalData = 0;// 電流n次加總初始設定為0
  VTotalData = 0;// 電壓n次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0
  CCVIrms = 0;//電流校正初始值為0

  start_time = micros();
  while((micros()-start_time) <= DataTime) {// 取樣時間16.6667ms 1s=1000000
    delayMicroseconds(57);// FOR TWO ADC IN 16.667MS
    IreadValue = analogRead(IPin);// 讀取電流ADC
    delayMicroseconds(57);// FOR TWO ADC IN 16.667MS
    VreadValue = analogRead(VPin);// 讀取電壓ADC

    //PreadValueForI = ((IreadValue * 3.3)/4095 - 1.52);// mean 1.52
    //PreadValueForV = ((VreadValue * 3.3)/4095 - 1.41);// meam 1.41
    //PreadValue = PreadValueForI * PreadValueForV;
    //PreadValue = (IreadValue * VreadValue);
    PreadValue = (((IreadValue * 3.3/4095) - 1.65) * ((VreadValue * 3.3/4095) - 1.65));
    /*PreadValueForI = ((IreadValue * 3.3/4095) - 1.65);
    PreadValueForV = ((VreadValue * 3.3/4095) - 1.65);*/
    //PreadValueForI = (IreadValue - 2048);
    //PreadValueForV = (VreadValue - 2048);
    //PreadValue = PreadValueForI * PreadValueForV;
    //PreadValue = ((IreadValue * VreadValue * 10.98/16769025) - 2.7225);
    //PreadValue = ((IreadValue) - 2048) * ((VreadValue) - 2048);
    if(IreadValue > ImaxValue) {// 電流ADC判斷大小值
      ImaxValue = IreadValue;
      }
    if(IreadValue < IminValue) {
      IminValue = IreadValue;
    }
      
    if(VreadValue > VmaxValue) {// 電壓ADC判斷大小值
      VmaxValue = VreadValue;
    }
    if(VreadValue < VminValue) {
      VminValue = VreadValue;
    }

    if(PreadValue > PmaxValue) {// PF判斷大小值
      PmaxValue = PreadValue;
    }
    if(PreadValue < PminValue) {
      PminValue = PreadValue;
    }

/*
    if(PreadValueForI > PreadValueForImax) {// PF判斷大小值
      PreadValueForImax = PreadValueForI;
    }
    if(PreadValueForI < PreadValueForImin) {
      PreadValueForI = PreadValueForImin;
    }

    if(PreadValueForV > PreadValueForVmax) {// PF判斷大小值
      PreadValueForVmax = PreadValueForV;
    }
    if(PreadValueForV < PreadValueForVmin) {
      PreadValueForV = PreadValueForVmin;
    }
     */
     //Status();
  }

  
  /*float ImaxValueForPmax = ((ImaxValue * 3.3)/4095 - 1.52 );
  float VmaxValueForPmax = ((VmaxValue * 3.3)/4095 - 1.41 );
  float IminValueForPmin = ((IminValue * 3.3)/4095 - 1.52 );
  float VminValueForPmin = ((VminValue * 3.3)/4095 - 1.41 );*/

  
  Iresult = ((ImaxValue - IminValue) * 3.3)/4095;
  Vresult = ((VmaxValue - VminValue) * 3.3)/4095;
  //Presult = ((PmaxValue - PminValue) * 2.7225)/4194304;// 求P
  Presult = ((PmaxValue - PminValue));// 求P
  //PageTwo(PmaxValue, PminValue);
  //PageTwo((PreadValueForImax*PreadValueForVmax), (PreadValueForImin*PreadValueForVmin));
  //PageTwo(PmaxValue, PminValue);
  //PageTwo(PmaxValue, PminValue);
  //Presult = ((ImaxValueForPmax * VmaxValueForPmax) - (IminValueForPmin * VminValueForPmin));// 求P
  //Presult = (PmaxValue);// 求P
  //Serial.println(count);
  
  Irms = ((Iresult/2) * 0.707 * 22.727);// 30A for 66 22.727
  
  // 電流校整參數
  if(0.5 <= Irms <= 3){// 當電流rms值在0.5~3安培時
    CCVIrms = Irms * CurrentCorrectionValuePointFiveToThree;
    CurrentStatus = 1;
    //PCorrectionValue = (((ImaxValueForPmax * 22.727 * CurrentCorrectionValuePointFiveToThree) * (VmaxValueForPmax * 257.13)) - ((IminValueForPmin * 22.727 * CurrentCorrectionValuePointFiveToThree) * (VminValueForPmin * 257.13)))/2;
    //PCorrectionValue = (((IminValueForPmin * 22.727 * CurrentCorrectionValuePointFiveToThree) * (VminValueForPmin * 257.13)) - ((ImaxValueForPmax * 22.727 * CurrentCorrectionValuePointFiveToThree) * (VmaxValueForPmax * 257.13)))/4;
    //PCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValuePointFiveToThree * 257.13);// for P
    //PageTwo(CurrentStatus, PCorrectionValue);
  }else if(3 < Irms <= 20){// 當電流rms值在3~20安培時
    CCVIrms = Irms * CurrentCorrectionValueThreeToTwenty;
    //PCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValueThreeToTwenty * 257.13);// for P
    CurrentStatus = 2;
    //PageTwo(CurrentStatus, PCorrectionValue);
    //PCorrectionValue = (((IminValueForPmin * 22.727 * CurrentCorrectionValueThreeToTwenty) * (VminValueForPmin * 257.13)) - ((ImaxValueForPmax * 22.727 * CurrentCorrectionValueThreeToTwenty) * (VmaxValueForPmax * 257.13)))/4;
    //PCorrectionValue = (((ImaxValueForPmax * 22.727 * CurrentCorrectionValueThreeToTwenty) * (VmaxValueForPmax * 257.13)) - ((IminValueForPmin * 22.727 * CurrentCorrectionValueThreeToTwenty) * (VminValueForPmin * 257.13)))/2;
  }else{
     CCVIrms = Irms * 0;
     //PCorrectionValue = ((Presult/2) * 22.727 * 0 * 257.13);// for P
     CurrentStatus = 3;
     //PageTwo(CurrentStatus, PCorrectionValue);
     //PCorrectionValue = (((ImaxValueForPmax * 22.727 * 0) * (VmaxValueForPmax * 257.13)) - ((IminValueForPmin * 22.727 * 0) * (VminValueForPmin * 257.13)))/4;
  }

  Vrms = ((Vresult/2) * 0.707 * 257.13);// 電壓參數校整
  
  PageOne(CCVIrms, Vrms);// 液晶螢幕顯示電壓有效值、電流有效值
  
  S = (CCVIrms * Vrms); 
  if(CurrentStatus == 1){
    PCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValuePointFiveToThree * 257.13) * 0.0178;
  }else if(CurrentStatus == 2){
    PCorrectionValue = ((Presult/2) * 22.727 * CurrentCorrectionValueThreeToTwenty * 257.13);
  }else if(CurrentStatus == 3){
    PCorrectionValue = ((Presult/2) * 22.727 * 257.13 * 0);
  }
  PF = (PCorrectionValue/S);
  
  PageTwo(PCorrectionValue, PF);// 液晶螢幕顯示消耗功率、功率因數
  //PFCorrectionValue = ((Presult/2)/((Iresult/2*0.707)*(Vresult/2*0.707)));
  /*OLEDRms = String("rms");
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
  }*/
}


/*int SR(){
  SRStatus = digitalRead(SRPin);
  if(SRStatus != AfterSRStatus){
    Serial.println(SRStatus);
  }else{
    AfterSRStatus = SRStatus;
  }

}*/

int SeverRelayStatus(){
    Inf = "http://120.126.8.126/api/switch/" + APIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
    HTTPClient http;// Declare object of class HTTPClient
    //http.begin("http://120.126.8.126/api/switch/07aaab96-b507-4afc-9d1a-3175dd653d08?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");
    http.begin(Inf);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.GET();// Send the request
    //Serial.println(http.getString());
    String JSONMessage = {http.getString()}; // Original message
    String JSONName = JSONMessage;
    String JSONData = JSONMessage;
    String SubstringJSONName = JSONName.substring(43, 49);// get item name
    String SubstringJSONData = JSONData.substring(51, 52);// get item data
    String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
    if(SubstringJSONData == "0"){
      return 0;
    }else if(SubstringJSONData == "1"){
      return 1;
    }else{
      
    }
    //Serial.println(DeviceStatus);
}

int Relay(){
  
  int ButtonOneStatus = digitalRead(ButtonOne);
  int ButtonTwoStatus = digitalRead(ButtonTwo);
  if(ButtonOneStatus == 0 && ButtonTwoStatus == 0){// 網路控制
    if(SeverRelayStatus() == 0){// 網路關閉繼電器
      digitalWrite(RelayControl, HIGH);
      LEDOn(GLED);
      LEDOff(YLED);
      LEDOff(RLED);
    }else if(SeverRelayStatus() == 1){// 網路開啟繼電器
      digitalWrite(RelayControl, LOW);
      LEDOff(GLED);
      LEDOn(YLED);
      LEDOff(RLED);
    }else{// 異常
      digitalWrite(RelayControl, HIGH);
      LEDOff(GLED);
      LEDOff(YLED);
      LEDOn(RLED);
    }
  }else if(ButtonOneStatus == 1){// 本地控制
    int ButtonTwoStatus = digitalRead(ButtonTwo);
    if(ButtonTwoStatus == 1){// 開啟電器
      digitalWrite(RelayControl, LOW);
      LEDOff(GLED);
      LEDOn(YLED);
      LEDOff(RLED);
    }else if(ButtonTwoStatus == 0){// 關閉繼電器
      digitalWrite(RelayControl, HIGH);
      LEDOn(GLED);
      LEDOff(YLED);
      LEDOff(RLED);
    }else{// 異常
      LEDOff(GLED);
      LEDOff(YLED);
      LEDOn(RLED);
    }
  }else{}
}

int OLEDRelayStatus(){
  display.setFont(ArialMT_Plain_24);//24 16 8
  int ButtonOneStatus = digitalRead(ButtonOne);
  int ButtonTwoStatus = digitalRead(ButtonTwo);
  // 控制方式
  if(ButtonOneStatus == 0){
    ControlTypeOLED = "Wi-Fi";
    if(SeverRelayStatus() == 0){
      RelayStausOLED = "OFF";
    }else if(SeverRelayStatus() == 1){
      RelayStausOLED = "ON";
    }else{
      RelayStausOLED = "ERROR";
    }
  }else if(ButtonOneStatus == 1){
    ControlTypeOLED = "Local";
    if(ButtonTwoStatus == 0){
      RelayStausOLED = "OFF";
    }else if(ButtonTwoStatus == 1){
      RelayStausOLED = "ON";
    }else{
      RelayStausOLED = "ERROR";
    }
  }else{}

  display.clear(); // clearing the display
  display.drawString(OLEDItemX, OLEDItemY, ControlTypeOLED);
  display.drawString(OLEDDataX, OLEDDataY, RelayStausOLED);
  display.display();  
}

int LEDOn(int LEDOnStatus){
  digitalWrite(LEDOnStatus, HIGH);
}

int LEDOff(int LEDOffStatus){
  digitalWrite(LEDOffStatus, LOW);
}


void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  //delay(1000);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "Init Setting");
    display.display();  
    Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();

  pinMode(5, OUTPUT);// LED
  pinMode(18, OUTPUT);// LED
  pinMode(19, OUTPUT);// LED
  pinMode(4, OUTPUT);// 繼電器
  pinMode(17, INPUT_PULLUP);// 按鍵B1
  pinMode(16, INPUT_PULLUP);// 按鍵B2
  pinMode(35, INPUT_PULLUP);// 電流訊號
  pinMode(34, INPUT_PULLUP);// 電壓訊號
  pinMode(33, INPUT_PULLUP);// 人體紅外線
  display.setFont(ArialMT_Plain_24);//24 16 8
}

void loop() {
  delay(500);
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    //RMS();
    Relay();
    OLEDRelayStatus();
    //SR();
    //int STATUS = digitalRead(33);
    //Serial.println(STATUS);
 }else{
  //Serial.println("Error in WiFi connection");
  //String ERR = String("Wi-Fi Fed Out");
    //display.clear(); // clearing the display
    //display.drawString(OLEDItemX, OLEDItemY, ERR);
    //display.display();   
 }
  //delay(500);
}
