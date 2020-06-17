#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// 儲存日期及時間的變數
String formattedDate;
String dayStamp;
String timeStamp;

// 設定無線基地台SSID跟密碼
const char* ssid     = "dlink-7730";
const char* password = "135791113";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {

  Serial.begin(9600);

  // 連接無線基地台
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // 等待連線，並從 Console顯示 IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

  // 設定時區 *60分 * 60秒，例如:
  // GMT +1 = 3600
  // GMT +8 = 28800  台灣時區
  // GMT 0 = 0
  timeClient.setTimeOffset(28800);
}

void loop() {

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // formattedDate函式取得的日期格式為： 2018-05-28T16:00:13Z
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // 取得日期
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);


  // 取得時間
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("TIME:");
  Serial.println(timeStamp);

 
  delay(1000);
}
