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
#define OLEDSreemHold 1000// 顯示資料保持1秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 0// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸
#define CurrentCorrectionValuePointFiveToThree 1.22637// 電流Irms在0.5~3 時, 校正參數
#define CurrentCorrectionValueThreeToTwenty 1.1819// 電流Irms在3~20 時, 校正參數
#define VoltagetCorrectionValueFull 0.9649// 電壓Vrms校正參數
const char* ssid = "dlink-7730";
const char* password =  "135791113";
String IP, MAC;

SH1106Wire display(OLEDAddress, SDA, SCL);
int IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount, count;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData, CCVIrms;
uint32_t start_time, FirstPage;
String OLEDI, OLEDV;



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
  IminValue = 4095;// 電流ADC讀取最大值初始設定為4096
  VminValue = 4095;// 電壓ADC讀取最大值初始設定為4096
  ITotalData = 0;// 電流n次加總初始設定為0
  VTotalData = 0;// 電壓n次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0
  CCVIrms = 0;//電流校正初始值為0
  //count = 0;
  //-for(DataCount = 0; DataCount < 10; DataCount++){
  start_time = micros();
  while((micros()-start_time) <= DataTime) {// 取樣時間16.6667ms 1s=1000000
    //delayMicroseconds(8250);// 取樣次數122次 delay8250 FOR 1S
    //delayMicroseconds(126);// 取樣次數122次 delay126 FOR 16.6667ms
    delayMicroseconds(58);// FOR TWO ADC IN 16.667MS
    IreadValue = analogRead(IPin);
    //VreadValue = analogRead(VPin);
    //delayMicroseconds(114);// 取樣次數100次 144
    //delayMicroseconds(4125);// FOR TWO ADC IN 1S
    delayMicroseconds(57);// FOR TWO ADC IN 16.667MS
    VreadValue = analogRead(VPin);
    
    //delayMicroseconds(4110);// FOR TWO ADC IN 1S
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
      //Status();
      //count++;
  }
  //Serial.println(count);
  //count = 0;
  Iresult = ((ImaxValue - IminValue) * 3.3)/4095;
  Vresult = ((VmaxValue - VminValue) * 3.3)/4095;
  //Serial.println(count);
  Irms = ((Iresult/2) * 0.707 * 22.727);// 30A for 66 22.727
  
  if(0.5 <= Irms <= 3){
    CCVIrms = Irms * CurrentCorrectionValuePointFiveToThree;
  }else if(3 < Irms <= 20){//當電流rms值在3~20安培時
    CCVIrms = Irms * CurrentCorrectionValueThreeToTwenty;
  }else{
     CCVIrms = Irms * 0;
  }
  
  //Irms = ((Iresult/2));// 30A for 66 22.727
  //Vrms = ((Vresult/2) * 0.707 * 257.13) * VoltagetCorrectionValueFull;
  Vrms = ((Vresult/2) * 0.707 * 257.13);
  //-ITotalData += Irms;
  //-VTotalData += Vrms;
  //-}
  //-IAvgData = (ITotalData/10);
  //-VAvgData = (VTotalData/10);
  //OLEDI = String("Irms: ") + Irms + String(" A"); 
  OLEDI = String("A = ") + CCVIrms;
  OLEDV = String("V = ") + Vrms;
  //OLEDI = String("I: ") + Irms;
  FirstPage = millis();

  String SImin = String(Iresult);
  
  while((millis() - FirstPage) < OLEDSreemHold){ //電流
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV);
    display.display();
    //Status();
  }

}

void setup() {
  //Serial.begin(9600);
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
