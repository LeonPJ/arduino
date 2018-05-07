#include <SoftwareSerial.h>
#include <IRremote.h>
IRsend irsend;  
#define DEBUG true

SoftwareSerial esp8266(7,8); //(RX,TX)
String ssid="MDFK";
String pwd="135791113";

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
  sendData("AT+RST\r\n",2000,DEBUG); // reset ESP8266
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); //configure as station
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); //enable multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",2000,DEBUG); //turn on server 80 port     
  while (!connectWifi(ssid, pwd)) {
    Serial.println("Connecting WiFi ... failed");
    delay(2000);
    }
  sendData("AT+GMR\r\n",1000,DEBUG);
  delay(3000); //wait for wifi connection to get local ip
  sendData("AT+CIFSR\r\n",1000,DEBUG); //get ip address
  }

void loop() {
  if (esp8266.available()) { // check if esp8266 is sending message
    int digitalRequests=0;
    int analogRequests=0;
    if (esp8266.find("+IPD,")) { //get client request
      delay(1000); //waiting for response: ?pinD2=1 or ?pinA4=1023
      int connectionId=esp8266.read()-48;  //turn ASCII to number
      if (esp8266.find("GET /?pin")) { //retrieve page name (router)
        char type=(char)esp8266.read(); //read char after ?pin
        int pin=esp8266.parseInt(); //get first int number from serial
        int val=esp8266.parseInt(); //get first int number from serial
        if (type=='D') {
          Serial.print("Digital pin ");
          pinMode(pin, OUTPUT); //set pin as ouput
          //digitalWrite(pin, val);
          //-----------------------------------------------------------
          switch(val)
      {
      case 2:
        irsend.sendNEC(0x807F8F70, 32);// decode
        break;

      case 3:
        irsend.sendNEC(0x807F0FF0, 32);
        break;

      case 0:
        irsend.sendNEC(0x807F7986, 32);
        break;

      case 1:
        irsend.sendNEC(0x807FBB44, 32);
        break;

      case 4:
        irsend.sendNEC(0x807FD32C, 32);
        break;

      case 5:
        irsend.sendNEC(0x807F3BC4, 32);
        break;

      case 6:
        irsend.sendNEC(0x807FFB04, 32);//fan type
        break;
        
      default:
        Serial.println("錯誤輸入");
        break;
      }
          //--------------------------------------------------------
          digitalRequests++;
          }

        /*else if (type=='A') {
          Serial.print("Analog pin ");
          analogWrite(pin, val);
          analogRequests++;             
          }*/
        else {
          Serial.println("Unexpected type:" + type);
          }
        Serial.print(pin);
        Serial.print("=");
        Serial.println(val);
        String webpage="<html>"; 
        webpage += (String)digitalRequests + " digital pin(s) written<br>";
        webpage += (String)analogRequests + " analog pin(s) written<br><br>";
        for (byte i=0; i<6; i++) {
          webpage += "A" + (String)i + "=" + (String)analogRead(i) + "<br>";
          }
        webpage += "</html>";
        String cipSend="AT+CIPSEND=";
        cipSend += connectionId;
        cipSend += ",";
        cipSend += webpage.length();
        cipSend += "\r\n";
        sendData(cipSend,1000,DEBUG);
        sendData(webpage,2000,DEBUG);
        sendData("AT+CIPCLOSE=" + (String)connectionId + "\r\n",3000,DEBUG);
        }
      }
    }
  }

boolean connectWifi(String ssid, String pwd) {
  String res=sendData("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"\r\n",8000,DEBUG);
  res.replace("\r\n",""); //remove all line terminator
  if (res.indexOf("OK") != -1) {return true;}
  else {return false;} 
  }

String sendData(String command, const int timeout, boolean debug) {
  String res="";
  esp8266.print(command);
  long int time=millis();
  while ((time + timeout) > millis()) {
    while(esp8266.available()) {res.concat((char)esp8266.read());}
    }
  if (debug) {Serial.print(res);}
  return res;
  }

