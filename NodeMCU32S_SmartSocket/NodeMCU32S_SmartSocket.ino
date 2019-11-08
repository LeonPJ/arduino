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
#define ButtonTwo 4// Button pin
#define RelayControl 16// Control Relay
#define DataTime 1000// Calculat Data Time

#define OLEDSreemHold 1500// 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸


SH1106Wire display(OLEDAddress, SDA, SCL);
int count;
float RMS() {
  float Iresult;
  float Vresult;

  int IreadValue;
  int VreadValue;

  int ImaxValue = 0;
  int VmaxValue = 0;

  int IminValue = 4096;
  int VminValue = 4096;
  count = 0;
  uint32_t start_time = millis();
  while((millis()-start_time) < DataTime) {//sample for 1 Sec
    //count ++;
    IreadValue = analogRead(IPin);
    //delayMicroseconds(1000);
    VreadValue = analogRead(VPin);
    //delayMicroseconds(1000);
    delayMicroseconds(8200);// 取樣次數122次
    if (IreadValue > ImaxValue) {
      ImaxValue = IreadValue;
    }
    if(IreadValue < IminValue) {
      IminValue = IreadValue;
    }

    if (VreadValue > VmaxValue) {
      VmaxValue = VreadValue;
    }
    if(VreadValue < VminValue) {
      VminValue = VreadValue;
    }

  }
  Iresult = ((ImaxValue - IminValue) * 3.3)/4096;
  Vresult = ((VmaxValue - VminValue) * 3.3)/4096;
  /*float VMAXresult = ((VmaxValue) * 3.3)/4096;
  float VMINresult = ((VminValue) * 3.3)/4096;*/
  float Irms = ((Iresult/2) * 0.707 * 22.9568);// 20A for 100; 30A for 66 
  float Vrms = ((Vresult/2) * 0.707 * 257.13);
  String OLEDI = String(" ") + Irms + String(" A"); 
  String OLEDV = String(" ") + Vrms + String(" V");
  //Serial.println(Vrms);
  
  //uint32_t FirstPage = millis();
  //while((millis() - FirstPage) < OLEDSreemHold) { //電流
    display.setFont(ArialMT_Plain_24);//24 16 10
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV);
    display.display();
   //}
  //return OLEDV;
  /*uint32_t FirstPage = millis();
  while((millis() - FirstPage) < OLEDSreemHold) { //電壓
    display.setFont(ArialMT_Plain_24);//24 16 10
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, OLEDI);
    display.drawString(OLEDDataX, OLEDDataY, OLEDV);
    display.display();
   }*/

  //return (Irms * Vrms);
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
  pinMode(4, INPUT_PULLUP);// 按鍵
  pinMode(14, INPUT_PULLUP);// 電流訊號
  pinMode(13, INPUT_PULLUP);// 電壓訊號
}

void loop() {
  //count = 0;
  //digitalWrite(16, HIGH);
  RMS();
  
  //delay(1000);
  /*digitalWrite(16, HIGH);
  display.clear(); // clearing the display
  display.drawString(0, 0, "1");
  display.display();*/
  /*delay(1000);
  digitalWrite(16, LOW);
  display.clear(); // clearing the display
  display.drawString(0, 0, "2");
  display.display();*/
}
