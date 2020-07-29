#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
//網路時間
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "dlink-7730";  // Enter SSID here
const char* password = "135791113"; //Enter Password here
String APIKEY = "80a8b60f-739e-4b89-99c2-dafe21bdd89a";
String formattedDate, dayStamp, timeStamp, IP, MAC, PostData, StatusData;
int count = 0;

#define InDoorPIN D5// 內側感測器
#define OutDoorPIN D6// 外側感測器

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int WifiLEDPrompt(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

int PopulationCount(){
  //delay(100);
  int STATUS;
  if((digitalRead(OutDoorPIN) > digitalRead(InDoorPIN)) && digitalRead(InDoorPIN) == LOW && STATUS != 2 && STATUS == 0){
    STATUS = 1;
    int BerforOutStaus = digitalRead(OutDoorPIN);
    while(digitalRead(InDoorPIN) == LOW && BerforOutStaus == HIGH){
      delay(10);
    }
    count++;
    Serial.println(count);
    PopulationCountData(count);
    delay(1000);
    OnStatus();
  }else if(digitalRead(OutDoorPIN) < digitalRead(InDoorPIN) && digitalRead(OutDoorPIN) == LOW && STATUS !=1 && STATUS == 0){
    STATUS = 2;
    int BerforInStaus = digitalRead(InDoorPIN);
    while(digitalRead(OutDoorPIN) == LOW && BerforInStaus == HIGH){
      delay(10);
    }
    count--;
    if(count == 0){
      OffStatus();
    }
    PopulationCountData(count);
    Serial.println(count);
    delay(1000);
  }else if(STATUS == 1 || STATUS == 2){
        STATUS = 0;
  }else{
    
  }

}

int PopulationCountData(int number){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();// formattedDate函式取得的日期格式為： 2018-05-28T16:00:13Z
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  String TIME = String(dayStamp + " " + timeStamp);
  
  PostData = "device_serial_number=" + APIKEY + "&category=controller&measure_item=人數&sign=person&value=" + number + "&timestamp=" + TIME;
  //Serial.println(PostData);
  HTTPClient http; // Declare object of class HTTPClient
  http.begin("http://120.126.8.126/api/record?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD"); // Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(PostData);  //Send the request
  //String JSONMessage = {http.getString()}; //Original message
  //String SubstringJSONMessage = JSONMessage.substring(41, 71);
  //Serial.println(SubstringJSONMessage);
  Serial.println(http.getString());
}

int OnStatus(){
  StatusData = "status=1";
  //Serial.println(PostData);
  HTTPClient http; // Declare object of class HTTPClient
  http.begin("http://120.126.8.126/api/switch/80a8b60f-739e-4b89-99c2-dafe21bdd89a?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD"); // Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(StatusData);  //Send the request
  Serial.println(http.getString());
}

int OffStatus(){
  StatusData = "status=0";
  //Serial.println(PostData);
  HTTPClient http; // Declare object of class HTTPClient
  http.begin("http://120.126.8.126/api/switch/80a8b60f-739e-4b89-99c2-dafe21bdd89a?api_token=60kSeSf78sXjr0W9AtZoxMC9vIj1gzZP8GcdtHtTIaELm7PteocrYzBgjFVD"); // Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
  int httpCode = http.POST(StatusData);  //Send the request
  Serial.println(http.getString());
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  timeClient.begin();
  timeClient.setTimeOffset(28800);  // 設定時區 *60分 * 60秒 GMT +8 = 28800 台灣時區
  
  pinMode(InDoorPIN, INPUT_PULLUP);
  pinMode(OutDoorPIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);                       // wait for a second
//digitalWrite(LED_BUILTIN, HIGH);
  if(WiFi.status() == WL_CONNECTED){   //Check WiFi connection status
    //WifiLEDPrompt();
    PopulationCount();
    
    //http.end();  //Close connection

 }else{
    Serial.println("Error in WiFi connection");   
 }
}
