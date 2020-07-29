//#ifndef UNIT_TEST
//#include <Arduino.h>
//#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define None 0 // 沒人
#define AreaOne 1 // 右1
#define AreaTwo 10 // 右2
#define AreaThree 100 // 右3

#define AreaOneADC 1 // 右1 ADC
#define AreaTwoADC 2 // 右2 ADC
#define AreaThreeADC 3 // 右3 ADC

const uint16_t kIrLed = 16;  // ESP8266 GPIO pin to use. D0
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

const char* ssid = "greenlab";  // Enter SSID here
const char* password = ""; //Enter Password here

String IP, MAC, Inf;

String RightLightFirstAPIKEY = "9231feae-a37e-425d-8c9a-7bf35f2afd53";
String RightLightSecondAPIKEY = "32e41e32-3abf-4b43-ba20-db33b5d82060";
String RightLightThirdAPIKEY = "90150a6e-e384-4f8a-b4eb-c5db06c7f0f2";
String PopulationAPIKEY = "80a8b60f-739e-4b89-99c2-dafe21bdd89a";

int FirstDeviceStatus, SecondDeviceStatus, ThirdDeviceStatus;

int FanOnOffStatus = 0;

//WiFiServer server(80);
//int i = 0;

int AreaJudgment(int FanArea){;
  if(FanArea == AreaOne){// 區域1
    IRSend(6);
  }else if(FanArea == AreaTwo){// 區域2
    IRSend(5);
  }else if(FanArea == AreaThree){// 區域3
    IRSend(7);
  }else if(FanArea == None){// 沒人
    IRSend(1);
  }else{
    IRSend(4);
  }
  
  
}

int IRSend(int STATUS){
  switch(STATUS){
    case 1:// 開/關
      irsend.sendNEC(0x80FFC13E);
      Serial.println("on/off");
      break;

    case 2:// 定時
      irsend.sendNEC(0x80FFD12E);
      Serial.println("set time");
      break;

    case 3:// 風速
      irsend.sendNEC(0x80FFE11E);
      Serial.println("fan");
      break;

    case 4:// 擺頭
      irsend.sendNEC(0x80FFF10E);
      Serial.println("move");
      break;

    case 5:// 擺中
      if(analogRead(A0) == AreaTwoADC){
        goto FiveBreak;
      }
      while(analogRead(A0) != AreaTwoADC){
        delay(100);
      }
      irsend.sendNEC(0x80FFF10E);
      FiveBreak:
      break;

    case 6:// 擺左
      if(analogRead(A0) == AreaOneADC){
        goto SixBreak;
      }
      while(analogRead(A0) != AreaOneADC){
        delay(100);
      }
      irsend.sendNEC(0x80FFF10E);
      SixBreak:
      break;

    case 7:// 擺右
      if(analogRead(A0) == AreaThreeADC){
        goto SevenBreak;
      }
      while(analogRead(A0) != AreaThreeADC){
        delay(100);
        Serial.println(analogRead(A0));
      }
      irsend.sendNEC(0x80FFF10E);
      SevenBreak:
      break;
    
    //case 10://new line ASCII
      //break;

    //case 13://carriage return ASCII
      //break;
        
    default:
      Serial.println("錯誤輸入");
      break;
    }
}

char RightLightFirst(){
  //Serial.println("connect...");
  //delay(2000);
  Inf = "http://120.126.8.126/api/switch/" + RightLightFirstAPIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
  HTTPClient http;// Declare object of class HTTPClient
  http.begin(Inf);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.GET();// Send the request
  //Serial.println(http.getString());
  String JSONMessage = {http.getString()}; // Original message
  String JSONName = JSONMessage;
  String JSONData = JSONMessage;
  String SubstringJSONName = JSONName.substring(43, 49);// get item name
  String SubstringJSONData = JSONData.substring(51, 52);// get item data
  String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
  //Serial.println(DeviceStatus);
  return (100*SubstringJSONData.toInt());
}

char RightLightSecond(){
  //Serial.println("connect...");
  //delay(2000);
  Inf = "http://120.126.8.126/api/switch/" + RightLightSecondAPIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
  HTTPClient http;// Declare object of class HTTPClient
  http.begin(Inf);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.GET();// Send the request
  //Serial.println(http.getString());
  String JSONMessage = {http.getString()}; // Original message
  String JSONName = JSONMessage;
  String JSONData = JSONMessage;
  String SubstringJSONName = JSONName.substring(43, 49);// get item name
  String SubstringJSONData = JSONData.substring(51, 52);// get item data
  String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
  //Serial.println(DeviceStatus);
  return (10*SubstringJSONData.toInt());
}

char RightLightThird(){
  //Serial.println("connect...");
  //delay(2000);
  Inf = "http://120.126.8.126/api/switch/" + RightLightThirdAPIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
  HTTPClient http;// Declare object of class HTTPClient
  http.begin(Inf);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.GET();// Send the request
  //Serial.println(http.getString());
  String JSONMessage = {http.getString()}; // Original message
  String JSONName = JSONMessage;
  String JSONData = JSONMessage;
  String SubstringJSONName = JSONName.substring(43, 49);// get item name
  String SubstringJSONData = JSONData.substring(51, 52);// get item data
  String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
  //Serial.println(DeviceStatus);
  return (1*SubstringJSONData.toInt());
}

int ThreeDeviceStatus(){
  FirstDeviceStatus = RightLightFirst();
  SecondDeviceStatus = RightLightSecond();
  ThirdDeviceStatus = RightLightThird();
  return (FirstDeviceStatus + SecondDeviceStatus + ThirdDeviceStatus);
}

char PopulationStaus(){
  //Serial.println("connect...");
  //delay(2000);
  Inf = "http://120.126.8.126/api/switch/" + PopulationAPIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
  HTTPClient http;// Declare object of class HTTPClient
  http.begin(Inf);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.GET();// Send the request
  //Serial.println(http.getString());
  String JSONMessage = {http.getString()}; // Original message
  String JSONName = JSONMessage;
  String JSONData = JSONMessage;
  String SubstringJSONName = JSONName.substring(43, 49);// get item name
  String SubstringJSONData = JSONData.substring(51, 52);// get item data
  String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
  //Serial.println(DeviceStatus);
  return (SubstringJSONData.toInt());
}

int WifiLEDErr(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void setup() {
  Serial.begin(9600);
  irsend.begin();
  delay(1000);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();  
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    if(PopulationStaus() == 1 && FanOnOffStatus == 0){
      IRSend(1);
      FanOnOffStatus == 1;
    }else if(PopulationStaus() == 0 && FanOnOffStatus == 1){
      IRSend(1);
      FanOnOffStatus == 0;
    }else{

    }
    AreaJudgment(ThreeDeviceStatus());
    delay(1000);
  }else{
    Serial.println("Error in WiFi connection");
    WifiLEDErr();
 }
  
}
