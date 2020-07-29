#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <SH1106Wire.h>

//網路時間
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define SRPin 33// SR-501 pin33


#define RLED 19// LED pin
#define GLED 18// LED pin
#define YLED 5// LED pin
#define RelayPin 4

const char* ssid = "dlink-7730";
const char* password = "135791113";
String IP, MAC, StatusData, Inf;

//String APIKEY = "15b05503-d145-4d35-8d8b-887486c79b91";// L1
//String APIKEY = "eb38799a-05df-4e9b-bb86-c7d8c116fbcc";// L2
//String APIKEY = "97bcc2a2-b69b-45ad-ad28-0df9d444059b";// L3

//String APIKEY = "9231feae-a37e-425d-8c9a-7bf35f2afd53";// R1
//String APIKEY = "32e41e32-3abf-4b43-ba20-db33b5d82060";// R2
String APIKEY = "90150a6e-e384-4f8a-b4eb-c5db06c7f0f2";// R3

String formattedDate, dayStamp, timeStamp;
int NowStatus;
int AfterStatus = 0;
int InitStatusData = 0;

int InitStatus(){
  if(InitStatusData == 0){
    for(int i = 0; i<3; i++){
      LEDOn(GLED);
      LEDOn(YLED);
      LEDOn(RLED);
      delay(500);
      LEDOff(GLED);
      LEDOff(YLED);
      LEDOff(RLED);
      delay(500);
    }
    InitStatusData = 1;
  }
}

int Relay(int RelayStatus){
  if(RelayStatus == 1){
    digitalWrite(RelayPin, HIGH);
    }else if(RelayStatus == 0){
      digitalWrite(RelayPin, LOW);
    }else{
      digitalWrite(RelayPin, LOW);
      LEDOff(GLED);
      LEDOff(YLED);
      LEDOn(RLED);
    }
  
  //digitalWrite(RelayPin, RelayStatus);
}

int LEDOn(int LEDOnStatus){
  digitalWrite(LEDOnStatus, HIGH);
}

int LEDOff(int LEDOffStatus){
  digitalWrite(LEDOffStatus, LOW);
}

int POSTData(int DATA){
  //delay(2000);
  if(DATA == 1){
    StatusData = "status=1";
  }else if(DATA == 0){
    StatusData = "status=0";
  }else{
    
  }
  Inf = "http://120.126.8.126/api/switch/" + APIKEY + "?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD";
  HTTPClient http;// Declare object of class HTTPClient
  http.begin(Inf);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(StatusData);// Send the request
  String JSONMessage = {http.getString()}; // Original message
  //String JSONName = JSONMessage;
  //String JSONData = JSONMessage;
  //String SubstringJSONName = JSONName.substring(43, 49);// get item name
  //String SubstringJSONData = JSONData.substring(51, 52);// get item data
  //String DeviceStatus = SubstringJSONName + String(":") + SubstringJSONData;// remix item name and data
  Serial.println(JSONMessage);
}

int SR(){
  NowStatus = digitalRead(SRPin);
  if(NowStatus != AfterStatus){
    AfterStatus = NowStatus;
    if(AfterStatus == LOW){// LOW是點燈
      Relay(1);
      Serial.println(NowStatus);
      POSTData(0);
      LEDOn(GLED);
      LEDOff(YLED);
      LEDOff(RLED);
    }else if(AfterStatus == HIGH){// HIGH是關燈
      Relay(0);
      Serial.println(NowStatus);
      POSTData(1);
      LEDOn(YLED);
      LEDOff(RLED);
      LEDOff(GLED);
    }else{
      Relay(0);
      LEDOn(RLED);
      LEDOff(YLED);
      LEDOff(GLED);
    }
    //Serial.println(AfterStatus);
  }else{
    //AfterSRStatus = SRStatus;
  }

}

void setup() {
  Serial.begin(9600);
  Relay(LOW);
  delay(1000);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  IP = WiFi.localIP().toString();// change to string
  MAC = WiFi.macAddress();

  timeClient.begin();
  timeClient.setTimeOffset(28800);  // 設定時區 *60分 * 60秒 GMT +8 = 28800 台灣時區
  
  pinMode(5, OUTPUT);// LED
  pinMode(18, OUTPUT);// LED
  pinMode(19, OUTPUT);// LED
  pinMode(4, OUTPUT);// 繼電器
  pinMode(33, INPUT_PULLUP);// 人體紅外線
}

void loop() {
  //InitStatus();
  //delay(500);
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    //Serial.println("online...");
    
    SR();
    
    /*while(!timeClient.update()) {
      timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();// formattedDate函式取得的日期格式為： 2018-05-28T16:00:13Z
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    String TIME = String(dayStamp + " " + timeStamp);
    Serial.println(TIME);*/
 }else{

 }
  delay(500);
}
