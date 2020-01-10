#include "ArduinoJson.h"

#include <WiFi.h>
#include <HTTPClient.h>

#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>

const char* ssid = "dlink-7730";
const char* password = "135791113";
String APIKEY = "8cedb77d-8a73-4083-8a98-54e522df7298";
String IP, MAC;
int Status;

//get internet time
unsigned int localPort = 2390;   //local port to listen for UDP packets
IPAddress timeServerIP;    //time.nist.gov NTP server address
const char* ntpServerName = "pool.ntp.org"; //NTP Server host name
const int NTP_PACKET_SIZE = 48;  //NTP timestamp resides in the first 48 bytes of packets
byte packetBuffer[ NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets
WiFiUDP udp;  //UDP instance to let us send and receive packets over UDP

//--------------------------------------------------------------------------------------
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
      //Serial.println("no packet yet");
      }
    else {  //received a packet, read the data from the buffer
      //Serial.print("packet received, length=");
      //Serial.println(cb);//=48 
      udp.read(packetBuffer, NTP_PACKET_SIZE);//read the packet into the buffer

      //the timestamp starts at byte 40 of the received packet and is four bytes,
      //or two words, long. First, esxtract the two words:
      unsigned long highWord=word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord=word(packetBuffer[42], packetBuffer[43]);
      //combine the four bytes (two words) into a long integer
      //this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900=highWord << 16 | lowWord;
      //Serial.print("Seconds since Jan 1 1900=" );
      //Serial.println(secsSince1900);
      //Serial.print("Unix time=");
      //Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      unix_time=secsSince1900 - 2208988800UL;
      //Serial.print(F("Unix time stamp (seconds since 1970-01-01)="));
      //Serial.println(unix_time); //print Unix time
      }
      return unix_time; //return seconds since 1970-01-01
}

unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("sending NTP packet...");
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

String GetTime() {
    //String d = getDate();
    //String t = getTime();
    String Time = getDate() + String(" ") + getTime();
    return Time;
}
//--------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);//connect to your local wi-fi network
  while (WiFi.status() != WL_CONNECTED) {//check wi-fi is connected to wi-fi network
    delay(500);
    Serial.print(".");
  }

  udp.begin(localPort);// 設定網路時間
  sync_clock();
  Alarm.timerRepeat(60, sync_clock); //timer task every 60 seconds
  
  //Serial.println("");
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();
  Serial.println("IP:" + IP + " MAC: " + MAC);
}

 
void loop() {
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    delay(5000);
    Serial.println("connect...");
    String SStatus = String(Status);
    delay(15000);
    String TIME = GetTime();
    //Serial.println(TIME);
    String POSTData = "device_serial_number=" + APIKEY + "&category=controller&measure_item=STATUS&sign=%2B&value=" + SStatus + "&timestamp=" + TIME;
    
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin("http://120.126.8.126/api/record?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
    int httpCode = http.POST(POSTData);  //Send the request
    //String payload = http.getString();    
    String JSONMessage = {http.getString()}; //Original message
    Serial.println(JSONMessage);
    
    Status++;
    StaticJsonBuffer<300> JSONBuffer; //Memory pool
    JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
    
    if(!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(5000);
      return;
    }
  const char * message = parsed["message"]; // get api key
  Serial.println(message);
  //delay(10000);
  }
}
//{"code":"000","message":"success","data":{"action":"create","category":"controller","id":5}}
