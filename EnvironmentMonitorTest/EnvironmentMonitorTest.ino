/*library*/
//NodeMCU library
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>
//ADS1115 library
#include <Wire.h>
#include <Adafruit_ADS1015.h>
//DHT22 library
#include <DHT.h>
//SSD1306 library
#include <SH1106Wire.h>
//----------------------------------------------------------------------------------------------------------------------------------------------
/*set*/
// set ADS1115
Adafruit_ADS1115 ads(0x48);  /* Use this for the 16-bit version */
// SET DHT
#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//set OLED
SH1106Wire display(0x3c, D2, D1);// D1 SCL, D2 SDA
//set Wi-Fi inf
char* ssid="dlink-7730";// WiFi SSID
char* password="135791113";// WiFi password
//get internet time
unsigned int localPort=2390;   //local port to listen for UDP packets
IPAddress timeServerIP;    //time.nist.gov NTP server address
const char* ntpServerName="pool.ntp.org"; //NTP Server host name
const int NTP_PACKET_SIZE=48;  //NTP timestamp resides in the first 48 bytes of packets
byte packetBuffer[ NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets
WiFiUDP udp;  //UDP instance to let us send and receive packets over UDP
//----------------------------------------------------------------------------------------------------------------------------------------------
//set OLED inf
#define OLEDSreemHold 1500// 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸
// Flame inf
#define FlamePin D5
// MQ9 inf
#define MQ9_PIN (ads.readADC_SingleEnded(0))// define which analog input channel you are going to use ads1115 -> ads.readADC_SingleEnded(0)
#define RL_VALUE (10)// define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83)// RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, which is derived from the chart in datasheet
#define CALIBARAION_SAMPLE_TIMES (50)//define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500)//define the time interal(in milisecond) between each samples in the cablibration phase
#define READ_SAMPLE_INTERVAL (50)//define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)//define the time interal(in milisecond) between each samples in 
/*#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)
float LPGCurve[3] = {2.3, 0.32, -0.47};
float COCurve[3] = {2.3, 0.24, -0.5};
float SmokeCurve[3] = {2.3,0.53,-0.44};*/
float Ro9 = 10;// Ro9 is initialized to 10 kilo ohms
// MG811 inf
#define MG811_PIN (ads.readADC_SingleEnded(1))
#define DC_GAIN (8.5)   //define the DC gain of amplifier 8.5
#define READ_SAMPLE_INTERVAL (50)    //define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)     //define the time interval(in milisecond) between each samples in normal operation
#define ZERO_POINT_VOLTAGE (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define REACTION_VOLTGAE (0.030) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
//float CO2Curve[3] = {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTGAE/(2.602-3))};
//----------------------------------------------------------------------------------------------------------------------------------------------
void sync_clock() {
  setTime(getUnixTime() + 28800L);
  }

String getDate() {
  String d=(String)year() + "-";
  byte M=month();
  if (M < 10) {
    d.concat('0');
    }
  d.concat(M);
  d.concat('-');
  byte D=day();
  if (D < 10) {
    d.concat('0');
    }
  d.concat(D);
  return d;
 }

String getTime() {
  String t="";
  byte h=hour();
  if (h < 10) {
    t.concat('0');
    }
  t.concat(h);
  t.concat(':');
  byte m=minute();
  if (m < 10) {
    t.concat('0');
    }
  t.concat(m);
  t.concat(':');
  byte s=second();
  if (s < 10) {
    t.concat('0');
    }
  t.concat(s);
  return t;
}

unsigned long getUnixTime() {
    WiFi.hostByName(ntpServerName, timeServerIP);//get a random server from the pool
    sendNTPpacket(timeServerIP);//send an NTP packet to a time server
    delay(1000);// wait to see if a reply is available

    int cb=udp.parsePacket();//return bytes received
    unsigned long unix_time=0;
    if (!cb) {
      Serial.println("no packet yet");
      }
    else {  //received a packet, read the data from the buffer
      Serial.print("packet received, length=");
      Serial.println(cb);//=48 
      udp.read(packetBuffer, NTP_PACKET_SIZE);//read the packet into the buffer

      //the timestamp starts at byte 40 of the received packet and is four bytes,
      //or two words, long. First, esxtract the two words:
      unsigned long highWord=word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord=word(packetBuffer[42], packetBuffer[43]);
      //combine the four bytes (two words) into a long integer
      //this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900=highWord << 16 | lowWord;
      Serial.print("Seconds since Jan 1 1900=" );
      Serial.println(secsSince1900);
      Serial.print("Unix time=");
      //Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      unix_time=secsSince1900 - 2208988800UL;
      Serial.print(F("Unix time stamp (seconds since 1970-01-01)="));
      Serial.println(unix_time); //print Unix time
      }
      return unix_time; //return seconds since 1970-01-01
}

unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  //Initialize values needed to form NTP request
  //(see URL above for details on the packets)
  packetBuffer[0]=0b11100011;   // LI, Version, Mode
  packetBuffer[1]=0;     // Stratum, or type of clock
  packetBuffer[2]=6;     // Polling Interval
  packetBuffer[3]=0xEC;  // Peer Clock Precision
  //8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]=49;
  packetBuffer[13]=0x4E;
  packetBuffer[14]=49;
  packetBuffer[15]=52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
//-------------------------------------------------------------

float MQResistanceCalculation(int raw_adc) {// 可能會有問題
  return ( ((float)RL_VALUE*(65536-raw_adc)/raw_adc));
}
 
float MQCalibration(int16_t mq_pin) {
  int i;
  float val = 0;
  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {// take multiple samples
    val += MQResistanceCalculation(mq_pin);// 可能會有問題
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;// calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;// divided by RO_CLEAN_AIR_FACTOR yields the Ro9  according to the chart in the datasheet 
  return val; 
}

float MQRead(int16_t mq_pin) {
  int i;
  float rs = 0;
  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(mq_pin);// 可能會有問題
    delay(READ_SAMPLE_INTERVAL);
    }
    rs = rs/READ_SAMPLE_TIMES;
    return rs;
    }
  
float MGRead(int mg_pin) {
  int i;
  float v = 0;
  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    v += (mg_pin * 0.1875 / 1000);
    delay(READ_SAMPLE_INTERVAL);
    }
  v = (v/READ_SAMPLE_TIMES);
  return v;  
}

/*int  MGGetPercentage(float volts, float *pcurve) {
  if ((volts/DC_GAIN ) >= ZERO_POINT_VOLTAGE) {
    return -1;
  } else { 
    return (pow(10, (((volts/DC_GAIN)-pcurve[1])/pcurve[2]) + pcurve[0]));
  }
}*/

void HumState()  {// 濕度狀態
  uint32_t FirstPage = millis();
  while((millis() - FirstPage) < OLEDSreemHold) { //濕度
    float hum = dht.readHumidity();
    String SHUM = hum + String(" %");
    display.setFont(ArialMT_Plain_24);//24 16 10
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "HUM:");
    display.drawString(OLEDDataX, OLEDDataY, SHUM);
    display.display();
   }
}

void TempState() {
  uint32_t SecondPage = millis();
  while((millis() - SecondPage) < OLEDSreemHold) {// 溫度
    float temp = dht.readTemperature();
    String STEMP = temp + String(" °C");
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "TEMP:");
    display.drawString(OLEDDataX, OLEDDataY, STEMP);
    display.display();
   }
}

void COState() {
  uint32_t ThirdPage = millis();
  while((millis() - ThirdPage) < OLEDSreemHold) {// 一氧化碳
    int CO;
    float Rrate = MQRead(MQ9_PIN)/Ro9;
    String SCO;
    //Serial.println(Rrate);
    if(1.73 >= Rrate && Rrate > 1.35){// 200ppm~300ppm
      CO = ((((-100) * (Rrate - 1.73))/(0.38)) + 200);
    }else if(1.35 >= Rrate && Rrate > 1.21){// 300ppm~400ppm
      CO = ((((-100) * (Rrate - 1.35))/(0.14)) + 300);
    }else if(1.21 >= Rrate && Rrate > 1.13){// 400ppm~500ppm
      CO = ((((-100) * (Rrate - 1.21))/(0.08)) + 400);
    }else if(1.13 >= Rrate && Rrate > 0.97){// 500ppm~600ppm
      CO = ((((-100) * (Rrate - 1.13))/(0.16)) + 500);
    }else if(0.97 >= Rrate && Rrate > 0.91){// 600ppm~700ppm
      CO = ((((-100) * (Rrate - 0.97))/(0.06)) + 600);
    }else if(0.91 >= Rrate && Rrate > 0.88){// 700ppm~800ppm
      CO = ((((-100) * (Rrate - 0.91))/(0.03)) + 700);
    }else if(0.88 >= Rrate && Rrate > 0.83){// 800ppm~900ppm
      CO = ((((-100) * (Rrate - 0.88))/(0.05)) + 800);
    }else if(0.83 >= Rrate && Rrate > 0.78){// 800ppm~1000ppm
      CO = ((((-100) * (Rrate - 0.83))/(0.05)) + 900);
    }else if(Rrate > 1.73){// lower 200ppm
      CO = 199;
    }else if(0.78 > Rrate){// over 1000ppm
      CO = 1000;
    }else{}
    
    if(CO > 1000){
      SCO = String("> 1000 ppm");
    }else if(CO <= 199){
      SCO = String("< 200 ppm");
    }else{
      SCO = CO + String(" ppm");
    }
    Serial.println(CO);
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, "CO:");
    display.drawString(OLEDDataX, OLEDDataY, SCO);
    display.display();
    }
}

void CO2State() {
  uint32_t FourthPage = millis();
  while((millis() - FourthPage) < OLEDSreemHold) {// 二氧化碳
    int CO2;
    String SCO2;
    float V = MGRead(MG811_PIN);
    float EMF = (V) * 100;
    Serial.println(V);
    if(323.64 >= EMF && EMF > 321.82){// 100ppm~200ppm
      CO2 = ((((-100) * (EMF - 323.64))/(1.82)) + 100);
    }else if(321.82 >= EMF && EMF > 320.91){// 200ppm~300ppm
      CO2 = ((((-100) * (EMF - 321.82))/(0.91)) + 200);
    }else if(320.91 >= EMF && EMF > 320.00){// 300ppm~400ppm
      CO2 = ((((-100) * (EMF - 320.91))/(0.91)) + 300);
    }else if(320.00 >= EMF && EMF > 319.09){// 400ppm~500ppm
      CO2 = ((((-100) * (EMF - 320.00))/(0.91)) + 400);
    }else if(319.09 >= EMF && EMF > 314.55){// 500ppm~600ppm
      CO2 = ((((-100) * (EMF - 319.09))/(4.54)) + 500);
    }else if(315.55 >= EMF && EMF > 310.91){// 600ppm~700ppm
      CO2 = ((((-100) * (EMF - 315.55))/(4.64)) + 600);
    }else if(310.91 >= EMF && EMF > 308.18){// 700ppm~800ppm
      CO2 = ((((-100) * (EMF - 310.91))/(1.91)) + 700);
    }else if(308.18 >= EMF && EMF > 305.45){// 800ppm~900ppm
      CO2 = ((((-100) * (EMF - 308.18))/(2.73)) + 800);
    }else if(305.45 >= EMF && EMF > 303.64){// 900ppm~1000ppm
      CO2 = ((((-100) * (EMF - 305.45))/(2.01)) + 900);
    }else if(303.64 >= EMF && EMF > 294.17){// 1000ppm~2000ppm
      CO2 = ((((-100) * (EMF - 303.64))/(9.47)) + 1000);
    }else if(294.17 >= EMF && EMF > 285.83){// 2000ppm~3000ppm
      CO2 = ((((-100) * (EMF - 294.17))/(8.34)) + 2000);
    }else if(285.83 >= EMF && EMF > 280.83){// 3000ppm~4000ppm
      CO2 = ((((-100) * (EMF - 285.83))/(5.00)) + 3000);
    }else if(280.83 >= EMF && EMF > 277.27){// 4000ppm~5000ppm
      CO2 = ((((-100) * (EMF - 280.83))/(3.56)) + 4000);
    }else if(277.27 >= EMF && EMF > 274.55){// 5000ppm~6000ppm
      CO2 = ((((-100) * (EMF - 277.27))/(2.72)) + 5000);
    }else if(274.55 >= EMF && EMF > 270.91){// 6000ppm~7000ppm
      CO2 = ((((-100) * (EMF - 274.55))/(3.64)) + 6000);
    }else if(270.91 >= EMF && EMF > 269){// 7000ppm~8000ppm
      CO2 = ((((-100) * (EMF - 270.91))/(1.91)) + 7000);
    }else if(269 >= EMF && EMF > 267){// 8000ppm~9000ppm
      CO2 = ((((-100) * (EMF - 269))/(2.00)) + 8000);
    }else if(267 >= EMF && EMF > 264){// 9000ppm~10000ppm
      CO2 = ((((-100) * (EMF - 267))/(3.00)) + 9000);
    }else if(264 > EMF ){// over 10000ppm
      CO2 = 10000;
    }else{// lower 100ppm
      CO2 = 100;
    }
    if(CO2 > 10000){
      SCO2 = String("> 10000 ppm");
    }else if(CO2 <= 99){
      SCO2 = String("< 100 ppm");
    }else{
      SCO2 = CO2 + String(" ppm");
    }
    Serial.println(SCO2);
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, "CO2:");
    display.drawString(OLEDDataX, OLEDDataY, SCO2);
    display.display();
    
  }
}

/*void CO2State() {
  uint32_t FourthPage = millis();
  while((millis() - FourthPage) < OLEDSreemHold) {// 二氧化碳
    int CO2;
    float volts;
    volts = MGRead(MG811_PIN);
    CO2 = MGGetPercentage(volts, CO2Curve);
    if(CO2 == -1) {
      String SCO2 = String("<400") + String(" ppm");
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "CO2:");
      display.drawString(OLEDDataX, OLEDDataY, SCO2);
      display.display();
      }else{
        String SCO2 = CO2 + String(" ppm");
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "CO2:");
        display.drawString(OLEDDataX, OLEDDataY, SCO2);
        display.display();
      }
    }
}*/

void LPGState() {
  uint32_t FifthPage = millis();
  while((millis() - FifthPage) < OLEDSreemHold) {// 液化石油氣
    int LPG;
    String SLPG;
    //float Rrate = MQRead(MQ9_PIN)/Ro9;
    float Rrate = 1.21;// 假的值
    //Serial.println(Rrate);
    if(2.11 >= Rrate && Rrate > 1.66){// 200ppm~300ppm
      LPG = ((((-100) * (Rrate - 2.11))/(0.45)) + 200);
    }else if(1.66 >= Rrate && Rrate > 1.5){// 300ppm~400ppm
      LPG = ((((-100) * (Rrate - 1.66))/(0.16)) + 300);
    }else if(1.5 >= Rrate && Rrate > 1.37){// 400ppm~500ppm
      LPG = ((((-100) * (Rrate - 1.5))/(0.13)) + 400);
    }else if(1.37 >= Rrate && Rrate > 1.27){// 500ppm~600ppm
      LPG = ((((-100) * (Rrate - 1.37))/(0.10)) + 500);
    }else if(1.27 >= Rrate && Rrate > 1.21){// 600ppm~700ppm
      LPG = ((((-100) * (Rrate - 1.27))/(0.06)) + 600);
    }else if(1.21 >= Rrate && Rrate > 1.13){// 700ppm~800ppm
      LPG = ((((-100) * (Rrate - 1.21))/(0.08)) + 700);
    }else if(1.13 >= Rrate && Rrate > 1.05){// 800ppm~900ppm
      LPG = ((((-100) * (Rrate - 1.13))/(0.08)) + 800);
    }else if(1.05 >= Rrate && Rrate > 1.00){// 900ppm~1000ppm
      LPG = ((((-100) * (Rrate - 1.05))/(0.05)) + 900);
    }else if(1.00 >= Rrate && Rrate > 0.72){// 1000ppm~2000ppm
      LPG = ((((-1000) * (Rrate - 1.00))/(0.28)) + 1000);
    }else if(0.72 >= Rrate && Rrate > 0.59){// 2000ppm~3000ppm
      LPG = ((((-1000) * (Rrate - 0.72))/(0.13)) + 2000);
    }else if(0.59 >= Rrate && Rrate > 0.51){// 3000ppm~4000ppm
      LPG = ((((-1000) * (Rrate - 0.59))/(0.08)) + 3000);
    }else if(0.51 >= Rrate && Rrate > 0.46){// 4000ppm~5000ppm
      LPG = ((((-1000) * (Rrate - 0.51))/(0.05)) + 4000);
    }else if(0.46 >= Rrate && Rrate > 0.43){// 5000ppm~6000ppm
      LPG = ((((-1000) * (Rrate - 0.46))/(0.03)) + 5000);
    }else if(0.43 >= Rrate && Rrate > 0.40){// 6000ppm~7000ppm
      LPG = ((((-1000) * (Rrate - 0.43))/(0.03)) + 6000);
    }else if(0.40 >= Rrate && Rrate > 0.37){// 7000ppm~8000ppm
      LPG = ((((-1000) * (Rrate - 0.40))/(0.03)) + 7000);
    }else if(0.37 >= Rrate && Rrate > 0.35){// 8000ppm~9000ppm
      LPG = ((((-100) * (Rrate - 0.37))/(0.02)) + 8000);
    }else if(0.35 >= Rrate && Rrate > 0.33){// 9000ppm~10000ppm
      LPG = ((((-1000) * (Rrate - 0.35))/(0.02)) + 9000);
    }else if(Rrate > 1.66 ){// lower 200ppm
      LPG = 199;
    }else if(0.33 > Rrate){// over 10000ppm
      LPG = 10000;
    }else{}
    
    if(LPG > 10000){
      SLPG = String("> 10000 ppm");
    }else if(LPG <= 199){
      SLPG = String("< 200 ppm");
    }else{
      SLPG = LPG + String(" ppm");
    }
    Serial.println(SLPG);
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, "LPG:");
    display.drawString(OLEDDataX, OLEDDataY, SLPG);
    display.display();
    }  
}

void FlameState() {// 火焰狀態
    uint32_t SixthPage = millis();
    while((millis() - SixthPage) < OLEDSreemHold) // 火苗偵測
    {
      int state = digitalRead(FlamePin);// 將感測器的值讀取给state
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "Flame State:");
        if(state == LOW) {
          String OLEDFLAME = String("Flame State: Discovery");
          display.drawString(OLEDDataX, OLEDDataY, "Discovery");
          }else {
            Serial.println("flame state: Normal");
            display.drawString(OLEDDataX, OLEDDataY, "Normal");
          }
         display.display();
        }
}

void TimeState() {
  uint32_t SixPage = millis();
  while((millis() - SixPage) < OLEDSreemHold) {// 液化石油氣
    String d = getDate();
    String t = getTime();
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, d);
    display.drawString(OLEDDataX, OLEDDataY, t);
    display.display();
    }  
}

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);//24 16 10
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.clear();
  display.drawString(OLEDItemX, OLEDItemY, "Initial Setting");
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println(WiFi.localIP());
  udp.begin(localPort);
  Serial.println(udp.localPort());
  sync_clock();
  Alarm.timerRepeat(60, sync_clock); //timer task every 60 seconds
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 10
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  // MQ9
  Ro9 = MQCalibration(MQ9_PIN);
}

void loop() {
   //HumState();
   //TempState();
   COState();
   //CO2State();
   LPGState();
   //FlameState();
   //TimeState();
}
 
