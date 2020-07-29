/*library*/
//NodeMCU library
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>
//ADS1115 library
#include <Wire.h>
#include <ADS1115.h>
//SSD1306 library
#include <SH1106Wire.h>
//----------------------------------------------------------------------------------------------------------------------------------------------
/*set*/
// set ADS1115
ADS1115 ads;
//set OLED
SH1106Wire display(0x3c, D3, D4);// D1 SCL, D2 SDA
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

void Vrms() {
  uint32_t VrmsPage = millis();
  //while((millis() - VrmsPage) < OLEDSreemHold) {// 電壓
    float result;
    float maxValue = 1.00;// store max value here
    float minValue = 3.30;// store min value here
    uint32_t start_time = millis();// millis(millis second)
      while((millis()-start_time) <= 2500) {//sample for 1 Sec
        float readVValue = (ads.Measure_SingleEnded(0) * 0.1875 /1000);
        Serial.println(readVValue);
        if (readVValue > maxValue && readVValue >= 2.3) { 
          maxValue = readVValue;// record the maximum sensor value
        }else if (readVValue < minValue && readVValue <= 2.3) {
          minValue = readVValue;// record the maximum sensor value
        }
      }
      /*Serial.println("++++++++++++++++++++++++++++++++++");
     Serial.print("max:");
      Serial.println(maxValue);
      Serial.print("min:");
      Serial.println(minValue);
       Serial.println("----------------------------------");*/
      result = (maxValue - minValue);// Subtract min from max
      //Serial.println(result);
      float Vrms = ((result / 2) * 0.707 * 695);
      String SVrms = Vrms + String(" V");
      Serial.println(SVrms);
      display.setFont(ArialMT_Plain_24);//24 16 10
      display.clear(); // clearing the display
      display.drawString(OLEDItemX, OLEDItemY, "Vrms:");
      display.drawString(OLEDDataX, OLEDDataY, SVrms);
      display.display();
  //}
}

void Irms() {
  uint32_t IrmsPage = millis();
  while((millis() - IrmsPage) < OLEDSreemHold) {// 電流
    float result;
    float maxValue = 0.50;// store max value here
    float minValue = 4.50;// store min value here
    uint32_t start_time = millis();// millis(millis second)
    while((millis() - start_time) < 2500) {// sample for 1 Sec
      float readAValue = (ads.Measure_SingleEnded(1) * 0.1875/1000);
      if (readAValue > maxValue) {
        maxValue = readAValue;// record the maximum sensor value
      }
      if(readAValue < minValue) {
        minValue = readAValue;// record the maximum sensor value
      }
    }
    result = (maxValue - minValue);// Subtract min from max
    float Irms = (result/2.0) * 0.707;
    float AmpsRMS = (Irms * 1000)/100;
    String SIrms = AmpsRMS + String(" A");
    Serial.println(SIrms);
    display.setFont(ArialMT_Plain_24);//24 16 10
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "Irms:");
    display.drawString(OLEDDataX, OLEDDataY, SIrms);
    display.display();
   }
}

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

void TimeState() {
  uint32_t SixPage = millis();
  while((millis() - SixPage) < OLEDSreemHold) {
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
  pinMode(D5, INPUT);// local control
  pinMode(D3, OUTPUT);// relay control
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
  
  ads.setGain(GAIN_TWOTHIRDS);// 2/3x gain +/- 6.144V  1 bit = 0.1875mV
  //ads.setGain(GAIN_TWO);          // 2x gain   +/- 2.048V  1 bit = 0.0625mV (default)
  ads.setMode(MODE_CONTIN);// Continuous conversion mode
  ads.setRate(RATE_128);// 128SPS (default)
  ads.setOSMode(OSMODE_SINGLE);// Set to start a single-conversion
  ads.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 10 
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
    digitalWrite(D3, LOW);
  int ButtomControlState = digitalRead(D5);
  // 接合0 打開1
  if(ButtomControlState == HIGH){
    digitalWrite(D3, LOW);// buttom off
    }else if(ButtomControlState == LOW){
      digitalWrite(D3, HIGH);// buttom on
      }else{
        
      }
  //Irms();
  Vrms();
  //TimeState();
}
