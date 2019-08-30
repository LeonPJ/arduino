#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>

ESP8266WebServer server(80); // 80 is the port number
WiFiServer server(80);

char ssid[] = "Green-7730";               // SSID of your home WiFi
char pass[] = "135791113";               // password of your home WiFi

IPAddress ip(192, 168, 0, 80);            // IP address of the server
IPAddress gateway(192,168,0,1);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network

String Website, data, XML, Javascript;
float inc = 0;

void javascriptContent(){
    Javascript ="<SCRIPT>\n";
    Javascript+="var xmlHttp=createXmlHttpObject();\n";
    Javascript+="function createXmlHttpObject(){\n";
    Javascript+="if(window.XMLHttpRequest){\n";
    Javascript+="xmlHttp=new XMLHttpRequest();\n";
    Javascript+="}else{\n";
    Javascript+="xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
    Javascript+="}\n";
    Javascript+="return xmlHttp;\n";
    Javascript+="}\n";
    Javascript+="\n";
    Javascript+="function response(){\n";
    Javascript+="xmlResponse=xmlHttp.responseXML;\n";
    Javascript+="xmldoc = xmlResponse.getElementsByTagName('data');\n";
    Javascript+="message = xmldoc[0].firstChild.nodeValue;\n";
    Javascript+="document.getElementById('div1').innerHTML=message;\n";
    Javascript+="}\n";

    Javascript+="function process(){\n";
    Javascript+="xmlHttp.open('PUT','xml',true);\n";
    Javascript+="xmlHttp.onreadystatechange=response;\n";
    Javascript+="xmlHttp.send(null);\n";
    Javascript+="setTimeout('process()',200);\n";
    Javascript+="}\n";
    Javascript+="</SCRIPT>\n";
  }

void WebsiteContent(){
    javascriptContent();
    
    Website="<html>\n";
    Website+="<style>\n";
    Website+="#div1{\n";
    Website+="width:400px;\n";
    Website+="margin:0 auto;\n";
    Website+="margin-top:130px;\n";
    Website+="font-size:900%;\n";
    Website+="color:powderblue;\n";
    Website+="}\n";
    Website+="</style>\n";
    Website+="<body onload='process()'>";
    Website+="<div id='div1'>"+data+"</div></body></html>";
    Website+=Javascript;
    server.send(200,"text/html",Website);
  }
void XMLcontent(){

  XML ="<?xml version='1.0'?>";
  XML+="<data>";
  XML+=data;
  XML+="</data>";
  server.send(200,"text/xml",XML);
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  Serial.print(WiFi.localIP());
  server.on("/",WebsiteContent);
  server.on("/xml",XMLcontent);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println(".");
      String request = client.readStringUntil('\r');// receives the message from the client
      Serial.print("From client: ");
      Serial.println("this is temp " + request);
      client.flush();
      client.println("Hi client! No, I am listening.\r");// sends the answer to the client
    }
    client.stop();// tarminates the connection with the client
  }
  data = (String)request;
  server.handleClient();

}
