#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "dlink-7730";
const char* password = "135791113";

//int ledPin = 13;

int timezone = 8 * 3600;//設定時區因為台灣是+8所以->時區*3600
int dst = 0;

int YEAR, MONTH, DAY, HOUR, MIN, SEC;

String TIME;

void setup() {
  //pinMode(ledPin,OUTPUT);
  //digitalWrite(ledPin,LOW);
  Serial.begin(9600);
  //Serial.println();
  //Serial.print("Wifi connecting to ");
  //Serial.println( ssid );
  WiFi.begin(ssid,password);
  //Serial.println();
  //Serial.print("Connecting");
  while( WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");      
  }
  //digitalWrite( ledPin , HIGH);
  //Serial.println();

  //Serial.println("Wifi Connected Success!");
  //Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );

  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  //Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  //Serial.println("\nTime response....OK");  
}

void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  /*Serial.print(p_tm->tm_mday);// 號
  Serial.print("/");
  Serial.print(p_tm->tm_mon + 1);// 月份
  Serial.print("/");
  Serial.print(p_tm->tm_year + 1900);// 年分
  Serial.print(" ");
  Serial.print(p_tm->tm_hour);// 小時
  Serial.print(":");
  Serial.print(p_tm->tm_min);// 分鐘
  Serial.print(":");
  Serial.println(p_tm->tm_sec);//秒*/
  YEAR = (p_tm->tm_year + 1900);
  MONTH = (p_tm->tm_mon + 1);
  DAY = (p_tm->tm_mday);
  HOUR = (p_tm->tm_hour);
  MIN = (p_tm->tm_min);
  SEC = (p_tm->tm_sec);
  TIME = String(YEAR + String("-") + MONTH + String("-") + DAY + String(" ") + HOUR + String(":") + MIN + String(":") + SEC);
  Serial.println(TIME);
}
