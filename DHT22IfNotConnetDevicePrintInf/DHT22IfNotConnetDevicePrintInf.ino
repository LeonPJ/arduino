#include "DHT.h"    // 使用 ESP8266 自帶的 DHT11 函式庫
                    // 如果 libraries 已有其他的 DHT 函式庫，請先移除
#include <ESP8266WiFi.h>

//*-- Hardware Serial
#define _baudrate   9600

//*-- DHT11
#define _dhtpin     D8
#define _dhttype    DHT11

// Initialize DHT sensor 
DHT dht11( _dhtpin, _dhttype );
float temperature, humidity;

//*-- IoT Information
#define SSID    "dlink-7730"
#define PASS    "135791113"
#define HOST    "api.thingspeak.com" // ThingSpeak IP Address: 184.106.153.149
#define PORT    80
String APIKEY  = "7U1B3P66P0UIGULC";
// 使用 GET 傳送資料的格式
// GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&filed2=[data 2]...;
String GET = "/update?key=7U1B3P66P0UIGULC";

void setup() {
    Serial.begin( _baudrate );
    Serial.println( "ESP8266 Ready!" );
    // Connecting to a WiFi network
    Serial.print("Connect to ");
    Serial.println( SSID );
    WiFi.begin( SSID, PASS );

    // 持續等待並連接到指定的 WiFi SSID
    while( WiFi.status() != WL_CONNECTED )
    {
        delay(500);
        Serial.print( "." );
    }
    Serial.println( "" );

    Serial.println( "WiFi connected" );
    Serial.println( "IP address: " );
    Serial.println( WiFi.localIP() );
    Serial.println( "" );
    
    // DHT11
    dht11.begin();
    Serial.println( "DHT11 Ready!" );
    delay(2000);
}

void loop() {
    humidity    = dht11.readHumidity();
    temperature = dht11.readTemperature();

    // 確認取回的溫溼度數據可用
    if( isnan( humidity ) || isnan( temperature ) )
    {
        Serial.println( "Failed to read form DHT11" );
    return;
    }
    else
    {
        // DHT11 溫度與濕度傳送      
        Serial.print( "Humidity: " );
        Serial.print( humidity );
        Serial.print( ", Temperature: " );
        Serial.println( temperature );
        updateDHT11();
    }

    // 每隔多久傳送一次資料
    delay( 20000 ); // 20 second
}

void updateDHT11()
{
    // 設定 ESP8266 作為 Client 端
    WiFiClient client;
    if( !client.connect( HOST, PORT ) )
    {
        Serial.println( "connection failed" );
        return;
    }
    else
    {
        // 準備上傳到 ThingSpeak IoT Server 的資料
        // 已經預先設定好 ThingSpeak IoT Channel 的欄位
        // field1：溫度；field2：濕度
        String postStr = APIKEY + "&field1=" + String((float)temperature) + "&field2=" + String((float)humidity) + "\r\n\r\n";

        client.print( "POST /update HTTP/1.1\n" );
        client.print( "Host: api.thingspeak.com\n" );
        client.print( "Connection: close\n" );
        client.print( "X-THINGSPEAKAPIKEY: "+APIKEY+"\n" );
        client.print( "Content-Type: application/x-www-form-urlencoded\n" );
        client.print( "Content-Length: " );
        client.print( postStr.length() );
        client.print( "\n\n" );
        client.print( postStr );

        client.stop();
    }
}
