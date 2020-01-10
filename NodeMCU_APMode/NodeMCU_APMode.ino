#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//定義 AP 名稱和密碼
const char *ssid = "WF8266R-AP";
const char *password = "1234567890";
//宣告 Web Server 服務
ESP8266WebServer server(80);

//Web Server 回應處理
void UP() {
  server.send(200, "text/html", F("<h1>Hi, I am WF8266R.</h1><a href='wf8266r?p1=hi&p2=wf8266r'>/wf8266r</a>"));
  digitalWrite(D0, HIGH);
}

void DOWN() {
  server.send(200, "text/html", F("<h1>Hi, I am WF8266R.</h1><a href='wf8266r?p1=hi&p2=wf8266r'>/wf8266r</a>"));
  digitalWrite(D0, LOW);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());// IP
  serverEvent();//提供服務的網址
  server.begin();//啟用 SERVER
  pinMode(D0, OUTPUT);
}

void loop() {
  server.handleClient();//監聽連線
}

void serverEvent(){
  server.on("/", UP);//http://192.168.4.1
  server.on("/down", DOWN);
  /*server.on("/wf8266r", []() {     //http://192.168.4.1/wf8266r
    String p1="",p2="";
    server.sendHeader("Connection", "close");
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "p1")
        p1 = server.arg(i);
      if (server.argName(i) == "p2")
        p2 = server.arg(i);
    }

    String message = "{\"p1\":\"" + p1 + "\",\"p2\":\""+p2+"\"}";
    server.send(200, "text/html", message);
  });*/
}
