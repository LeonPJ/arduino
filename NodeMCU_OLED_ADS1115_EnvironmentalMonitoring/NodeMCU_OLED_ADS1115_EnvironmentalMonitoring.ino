/*library*/
//ADS1115 library
#include <Wire.h>
#include <ADS1115.h>
//DHT22 library
#include <DHT.h>
//SSD1306 library
#include <SH1106Wire.h>
//
/*set*/
// set ADS1115
ADS1115 ads;
// SET DHT
#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
//set OLED
SH1106Wire display(0x3c, D2, D1);// D1 SCL, D2 SDA
//set OLED inf
#define OLEDSreemHold 1500// 顯示資料保持1.5秒
#define OLEDItemX 0// 顯示OLED名稱 X軸
#define OLEDItemY 0// 顯示OLED名稱 Y軸
#define OLEDDataX 25// 顯示OLED數據 X軸
#define OLEDDataY 24// 顯示OLED數據 Y軸

#define FlamePin D5

#define MQ9_PIN (ads.Measure_SingleEnded(0))// define which analog input channel you are going to use ads1115 -> ads.readADC_SingleEnded(0)
#define MG811_PIN (ads.Measure_SingleEnded(1))

#define RL_VALUE (5)// define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR (9.83)// RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO, which is derived from the chart in datasheet
 
/**********************Software Related Macros***********************************/
#define CALIBARAION_SAMPLE_TIMES (50)//define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500)//define the time interal(in milisecond) between each samples in the cablibration phase
#define READ_SAMPLE_INTERVAL (50)//define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)//define the time interal(in milisecond) between each samples in 
 
/*********************Application Related Macros*********************************/
#define GAS_LPG (0)
#define GAS_CO (1)
#define GAS_SMOKE (2)
#define GAS_CO2 (3)
/****************************Globals**********************************************/
float LPGCurve[3] = {2.3,0.32,-0.46};
float COCurve[3] = {2.3,0.23,-0.49};
float SmokeCurve[3] = {2.3,0.53,-0.44};
float CO2Curve[3] = {2.6,0.20,-0.075};

float Ro9 = 10;// Ro9 is initialized to 10 kilo ohms
float Ro811 = 10;

float MQResistanceCalculation(int raw_adc) {
  return ( ((float)RL_VALUE*(65536-raw_adc)/raw_adc));
}
 
float MQCalibration(int mq_pin) {
  int i;
  float val=0;
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {// take multiple samples
    val += MQResistanceCalculation(mq_pin);
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;// calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;// divided by RO_CLEAN_AIR_FACTOR yields the Ro9  according to the chart in the datasheet 
  return val; 
}


float MQRead(int mq_pin) {
  int i;
  float rs=0;
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(mq_pin);
    delay(READ_SAMPLE_INTERVAL);
    }
    rs = rs/READ_SAMPLE_TIMES;
    return rs;
    }

int MQGetGasPercentage(float rs_ro_ratio, int gas_id) {
  if ( gas_id == GAS_LPG ) {
    return MQGetPercentage(rs_ro_ratio,LPGCurve);
    } else if ( gas_id == GAS_CO ) {
      return MQGetPercentage(rs_ro_ratio,COCurve);
      } else if ( gas_id == GAS_SMOKE ) {
        return MQGetPercentage(rs_ro_ratio,SmokeCurve);
        } else if ( gas_id == GAS_CO2 ) {
          return MQGetPercentage(rs_ro_ratio,CO2Curve);
          }
          return 0;
          }

int  MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
  }

void FlameState() {
    int state = digitalRead(FlamePin);// 將感測器的值讀取给state
    
    uint32_t SixthPage = millis();
    while((millis() - SixthPage) < OLEDSreemHold) // 火苗偵測
    {
      if(state == LOW) {
        String OLEDFLAME = String("flame state: Discovery");
        Serial.println(OLEDFLAME);
        display.clear();
        display.drawString(OLEDItemX, OLEDItemY, "Flame State: ");
        display.drawString(OLEDDataX, OLEDDataY, "Discovery");
        display.display();
        display.println(OLEDFLAME);//---------
        Serial.println("flame state: discovery flame");
      }else{
         String OLEDFLAME = String("flame state: normal");
         Serial.println(OLEDFLAME);
         display.clear();
         display.drawString(OLEDItemX, OLEDItemY, "Flame State: ");
         display.drawString(OLEDDataX, OLEDDataY, "Normal");
         display.display();
        }
    }

}

void setup() {
  Serial.begin(9600);
  ads.setGain(GAIN_TWOTHIRDS);// 2/3x gain +/- 6.144V  1 bit = 0.1875mV
  ads.setMode(MODE_CONTIN);// Continuous conversion mode
  ads.setRate(RATE_128);// 128SPS (default)
  ads.setOSMode(OSMODE_SINGLE);// Set to start a single-conversion
  ads.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//24 16 8
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  Ro9 = MQCalibration(MQ9_PIN);
  Ro811 = MQCalibration(MG811_PIN);
}
 
void loop() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  int CO = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_CO);
  int CO2 =  MQGetGasPercentage(MQRead(MG811_PIN)/Ro811,GAS_SMOKE);
  int LPG = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_LPG);
  //int SOMKE = MQGetGasPercentage(MQRead(MQ9_PIN)/Ro9,GAS_SMOKE);
  // print On OLED
  String SHUM = hum + String(" %");
  String STEMP = temp + String(" °C");
  String SCO = CO + String(" ppm");
  String SCO2 = CO2 + String(" ppm");
  String SLPG = LPG +  String(" ppm");
  // send to sever
  String OLEDHUM = String("hum: ") + hum + String(" %");
  String OLEDTEMP = String("temp: ") + temp + String(" Celsius");
  String OLEDCO = String("CO: ") + CO + String(" ppm");
  String OLEDCO2 = String("CO2: ") + CO2 + String(" ppm");
  String OLEDLPG = String("LPG: ") + LPG + String(" ppm");// Conversion target and combine string ("name" + target + "ppm")
  //String OLEDSOMKE = String("SOMKE: ") + SOMKE + String(" ppm");
  //String OLEDFLAME = String(FlameState());
  // print in serial
    Serial.println(OLEDHUM);
    Serial.println(OLEDTEMP);
    Serial.println(OLEDCO);
    Serial.println(OLEDCO2);
    Serial.println(OLEDLPG);
  //Serial.println(OLEDSOMKE);
  //FlameState();
  Serial.println("------------------");

   uint32_t FirstPage = millis();
   while((millis() - FirstPage) < OLEDSreemHold) //濕度
   {
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "HUM:");
    display.drawString(OLEDDataX, OLEDDataY, SHUM);
    display.display();
   }

   uint32_t SecondPage = millis();
   while((millis() - SecondPage) < OLEDSreemHold) // 溫度
   {
    display.clear(); // clearing the display
    display.drawString(OLEDItemX, OLEDItemY, "TEMP:");
    display.drawString(OLEDDataX, OLEDDataY, STEMP);
    display.display();
   }

    uint32_t ThirdPage = millis();
    while((millis() - ThirdPage) < OLEDSreemHold) // 一氧化碳
    {
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "CO:");
      display.drawString(OLEDDataX, OLEDDataY, SCO);
      display.display();
    }

    uint32_t FourthPage = millis();
    while((millis() - FourthPage) < OLEDSreemHold) // 二氧化碳
    {
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "CO2:");
      display.drawString(OLEDDataX, OLEDDataY, SCO2);
      display.display();
    }

    uint32_t FifthPage = millis();
    while((millis() - FifthPage) < OLEDSreemHold) // 液化石油氣
    {
      display.clear();
      display.drawString(OLEDItemX, OLEDItemY, "LPG:");
      display.drawString(OLEDDataX, OLEDDataY, SLPG);
      display.display();
    }

    FlameState();
}
