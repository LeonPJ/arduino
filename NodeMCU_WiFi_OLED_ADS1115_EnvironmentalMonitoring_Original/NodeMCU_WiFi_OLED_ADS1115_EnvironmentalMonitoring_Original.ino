/*library*/
//NodeMCU library
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>
//ADS1115 library
#include <Wire.h>
#include <ADS1115.h>
//DHT22 library
#include <DHT.h>
//SSD1306 library
#include <SH1106Wire.h>
//
/*set*/
// set ADS1115
ADS1115 ads;
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
//set OLED inf
#define OLEDSreemHold 1500// 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸

#define FlamePin D5

#define MQ9_PIN (ads.Measure_SingleEnded(0))// define which analog input channel you are going to use ads1115 -> ads.readADC_SingleEnded(0)
#define MG811_PIN (ads.Measure_SingleEnded(1))

#define RL_VALUE (5)// define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83)// RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, which is derived from the chart in datasheet
 
/**********************Software Related Macros***********************************/
#define CALIBARAION_SAMPLE_TIMES (50)//define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500)//define the time interal(in milisecond) between each samples in the cablibration phase
#define READ_SAMPLE_INTERVAL (50)//define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)//define the time interal(in milisecond) between each samples in 
 
/*********************Application Related Macros*********************************/
#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)
#define GAS_CO2 (3)
/****************************Globals**********************************************/
float LPGCurve[3] = {2.3,0.32,-0.46};
float COCurve[3] = {2.3,0.23,-0.49};
float SmokeCurve[3] = {2.3,0.53,-0.44};
float CO2Curve[3] = {2.6,0.20,-0.075};

float Ro9 = 10;// Ro9 is initialized to 10 kilo ohms
float Ro811 = 10;

//--------------------------------------------------
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
    WiFi.hostByName(ntpServerName, timeServerIP);  //get a random server from the pool
    sendNTPpacket(timeServerIP);                   //send an NTP packet to a time server
    delay(1000);                                   // wait to see if a reply is available

    int cb=udp.parsePacket();                      //return bytes received
    unsigned long unix_time=0;
    if (!cb) {
      Serial.println("no packet yet");
      }
    else {  //received a packet, read the data from the buffer
      Serial.print("packet received, length=");
      Serial.println(cb);                        //=48 
      udp.read(packetBuffer, NTP_PACKET_SIZE);  //read the packet into the buffer

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

float MQResistanceCalculation(int raw_adc) {
  return ( ((float)RL_VALUE*(65536-raw_adc)/raw_adc));
}
 
float MQCalibration(int mq_pin) {
  int i;
  float val=0;
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {// take multiple samples
    val += MQResistanceCalculation(mq_pin);
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;// calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;// divided by RO_CLEAN_AIR_FACTOR yields the Ro9  according to the chart in the datasheet 
  return val; 
}


float MQRead(int mq_pin) {
  int i;
  float rs=0;
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(mq_pin);
    delay(READ_SAMPLE_INTERVAL);
    }
    rs = rs/READ_SAMPLE_TIMES;
    return rs;
    }

int MQGetGasPercentage(float rs_ro_ratio, int gas_id) {
  if ( gas_id == GAS_LPG ) {
    return MQGetPercentage(rs_ro_ratio,LPGCurve);
    } else if ( gas_id == GAS_CO ) {
      return MQGetPercentage(rs_ro_ratio,COCurve);
      } else if ( gas_id == GAS_SMOKE ) {
        return MQGetPercentage(rs_ro_ratio,SmokeCurve);
        } else if ( gas_id == GAS_CO2 ) {
          return MQGetPercentage(rs_ro_ratio,CO2Curve);
          }
          return 0;
          }

int  MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
  }

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
    int CO = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_CO);
    String SCO = CO + String(" ppm");
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, "CO:");
    display.drawString(OLEDDataX, OLEDDataY, SCO);
    display.display();
    }
}

void CO2State() {
  uint32_t FourthPage = millis();
  while((millis() - FourthPage) < OLEDSreemHold) {// 二氧化碳
    int CO2 = MQGetGasPercentage(MQRead(MG811_PIN)/Ro811,GAS_SMOKE);
    String SCO2 = CO2 + String(" ppm");
    display.clear();
    display.drawString(OLEDItemX, OLEDItemY, "CO2:");
    display.drawString(OLEDDataX, OLEDDataY, SCO2);
    display.display();
    }  
}
void LPGState() {
  uint32_t FifthPage = millis();
  while((millis() - FifthPage) < OLEDSreemHold) {// 液化石油氣
    int LPG = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_LPG);
    String SLPG = LPG +  String(" ppm");
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
  
  ads.setGain(GAIN_TWOTHIRDS);// 2/3x gain +/- 6.144V  1 bit = 0.1875mV
  ads.setMode(MODE_CONTIN);// Continuous conversion mode
  ads.setRate(RATE_128);// 128SPS (default)
  ads.setOSMode(OSMODE_SINGLE);// Set to start a single-conversion
  ads.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 10
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  Ro9 = MQCalibration(MQ9_PIN);
  Ro811 = MQCalibration(MG811_PIN);
}




 
void loop() {
  //String d = getDate();
  //String t = getTime();

  //float hum = dht.readHumidity();
  //float temp = dht.readTemperature();
  //int CO = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_CO);
  //int CO2 =  MQGetGasPercentage(MQRead(MG811_PIN)/Ro811,GAS_SMOKE);
  //int LPG = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_LPG);
  //int SOMKE = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_SMOKE);
  // print On OLED
  //String SHUM = hum + String(" %");
  //String STEMP = temp + String(" °C");
  //String SCO = CO + String(" ppm");
  //String SCO2 = CO2 + String(" ppm");
  //String SLPG = LPG +  String(" ppm");
  // send to sever
  //String OLEDHUM = String("hum: ") + hum + String(" %");
  //String OLEDTEMP = String("temp: ") + temp + String(" °C");
  //String OLEDCO = String("CO: ") + CO + String(" ppm");
  //String OLEDCO2 = String("CO2: ") + CO2 + String(" ppm");
  //String OLEDLPG = String("LPG: ") + LPG + String(" ppm");// Conversion target and combine string ("name" + target + "ppm")
  //String OLEDSOMKE = String("SOMKE: ") + SOMKE + String(" ppm");
  //String OLEDFLAME = String(FlameState());
  // print in serial
    //Serial.println(OLEDHUM);
    //Serial.println(OLEDTEMP);
    //Serial.println(OLEDCO);
    //Serial.println(OLEDCO2);
    //Serial.println(OLEDLPG);
    //Serial.println(d);
    //Serial.println(t);
  //Serial.println(OLEDSOMKE);
  //FlameState();
  //Serial.println("------------------");

   /*uint32_t FirstPage = millis();
   while((millis() - FirstPage) < OLEDSreemHold) //濕度
   {
    float hum = dht.readHumidity();
    String SHUM = hum + String(" %");
    display.setFont(ArialMT_Plain_24);//24 16 10
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "HUM:");
    display.drawString(OLEDDataX, OLEDDataY, SHUM);
    display.display();
   }*/
   HumState();
   TempState();
   COState();
   CO2State();
   LPGState();
   FlameState();
   TimeState();
   /*uint32_t SecondPage = millis();
   while((millis() - SecondPage) < OLEDSreemHold) // 溫度
   {
    float temp = dht.readTemperature();
    String STEMP = temp + String(" °C");
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "TEMP:");
    display.drawString(OLEDDataX, OLEDDataY, STEMP);
    display.display();
   }*/

    /*uint32_t ThirdPage = millis();
    while((millis() - ThirdPage) < OLEDSreemHold) // 一氧化碳
    {
      int CO = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_CO);
      String SCO = CO + String(" ppm");
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "CO:");
      display.drawString(OLEDDataX, OLEDDataY, SCO);
      display.display();
    }*/

    /*uint32_t FourthPage = millis();
    while((millis() - FourthPage) < OLEDSreemHold) // 二氧化碳
    {
      int CO2 = MQGetGasPercentage(MQRead(MG811_PIN)/Ro811,GAS_SMOKE);
      String SCO2 = CO2 + String(" ppm");
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "CO2:");
      display.drawString(OLEDDataX, OLEDDataY, SCO2);
      display.display();
    }*/

    /*uint32_t FifthPage = millis();
    while((millis() - FifthPage) < OLEDSreemHold) // 液化石油氣
    {
      int LPG = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_LPG);
      String SLPG = LPG +  String(" ppm");
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "LPG:");
      display.drawString(OLEDDataX, OLEDDataY, SLPG);
      display.display();
    }*/

    

    /*uint32_t SixPage = millis();
    while((millis() - SixPage) < OLEDSreemHold) // 液化石油氣
    {
      String d = getDate();
      String t = getTime();
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, d);
      display.drawString(OLEDDataX, OLEDDataY, t);
      display.display();
    }*/
  
}
