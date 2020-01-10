#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//定義 AP 名稱和密碼
const char *ssid = "ID8266";
const char *password = "16161616";
//宣告 Web Server 服務
ESP8266WebServer server(80);

//Web Server 回應處理
/*void handleRoot() {
  server.send(200, "text/html", F("<h1>Hi, I am WF8266R.</h1><a href='wf8266r?p1=hi&p2=wf8266r'>/wf8266r</a>"));
}*/

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());    //伺服器 IP
  Serial.print("AP : ");
  Serial.println(ssid);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  serverEvent();//提供服務的網址
  server.begin();//啟用 SERVER
}

void loop() {
  server.handleClient();//監聽連線
  digitalWrite(LED_BUILTIN, HIGH);
}

void serverEvent(){
  server.on("/on", []() {     //http://192.168.4.1/on
    digitalWrite(D0, LOW);
  }); 

  server.on("/off", []() {     //http://192.168.4.1/off
    digitalWrite(D0, HIGH);
    /*String p1="",p2="";
    server.sendHeader("Connection", "close");
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "p1")
        p1 = server.arg(i);
      if (server.argName(i) == "p2")
        p2 = server.arg(i);
    }

    String message = "{\"p1\":\"" + p1 + "\",\"p2\":\""+p2+"\"}";
    server.send(200, "text/html", message);*/
  }); 
}
