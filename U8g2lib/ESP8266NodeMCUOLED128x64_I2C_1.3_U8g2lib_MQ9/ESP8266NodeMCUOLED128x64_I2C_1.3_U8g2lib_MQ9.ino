// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SH1106Wire.h>
#include <Adafruit_ADS1015.h>

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, D2, D1);
Adafruit_ADS1115 ads;

#define         MQ_PIN                       (ads.readADC_SingleEnded(0))
#define         RL_VALUE                     (5)
#define         RO_CLEAN_AIR_FACTOR          (9.83)
 
/**********************Software Related Macros***********************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)
#define         CALIBRATION_SAMPLE_INTERVAL  (500)
#define         READ_SAMPLE_INTERVAL         (50)
#define         READ_SAMPLE_TIMES            (5)

/*********************Application Related Macros*********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)
 
/****************************Globals**********************************************/
float           LPGCurve[3] = {2.3,0.32,-0.46};
float           COCurve[3] = {2.3,0.23,-0.49};
float           SmokeCurve[3] = {2.3,0.53,-0.44};
float           Ro           = 10;


float MQResistanceCalculation(int raw_adc) {
  return ( ((float)RL_VALUE*(65536-raw_adc)/raw_adc));
  }
  
float MQCalibration(int mq_pin) {
  int i;
  float val = 0;
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {
    val += MQResistanceCalculation(mq_pin);
    delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    val = val/CALIBARAION_SAMPLE_TIMES;
    val = val/RO_CLEAN_AIR_FACTOR;
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
        }
        return 0;
 }

int  MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

void setup() {
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setColor(WHITE);
  ads.begin();
  Ro = MQCalibration(MQ_PIN);
}

void MQ9LPG() {
  int LPG = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG);
  if(LPG < 100){
    String OLEDLPG = String(" < 100") + String(" ppm");
    display.drawString(0, 0, "LPG:");
    display.drawString(0, 32, OLEDLPG);
    display.display();
    display.clear();
    }else if(LPG > 10000) {
      String OLEDLPG =  String(" > 10000") + String(" ppm");
      display.drawString(0, 0, "LPG:");
      display.drawString(0, 32, OLEDLPG);
      display.display();
      display.clear();
      }else{
        String OLEDLPG = LPG + String(" ppm");
        display.drawString(0, 0, "LPG:");
        display.drawString(0, 32, OLEDLPG);
        display.display();
        display.clear();
        }
        delay(1000);
  }

void MQ9CO() {
  int CO =  MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO);
  if(CO < 10){
    String OLEDCO = String(" < 10") + String(" ppm");
    display.drawString(0, 0, "CO:");
    display.drawString(0, 32, OLEDCO);
    display.display();
    display.clear();
    }else if(CO > 1000) {
      String OLEDCO =  String(" > 1000") + String(" ppm");
      display.drawString(0, 0, "CO:");
      display.drawString(0, 32, OLEDCO);
      display.display();
      display.clear();
      }else{
        String OLEDCO = CO + String(" ppm");
        display.drawString(0, 0, "CO:");
        display.drawString(0, 32, OLEDCO);
        display.display();
        display.clear();
        }
        delay(1000);
  }

void MQ9SMOKE() {
  int SMOKE = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);
  String OLEDSMOKE = SMOKE + String(" ppm");
  display.drawString(0, 0, "SMOKE:");
  display.drawString(0, 32, OLEDSMOKE);
  display.display();
  display.clear();
  delay(1000);
}

void displayMQ9() {
  MQ9LPG();
  MQ9CO();
  MQ9SMOKE();
  //String OLEDLPG = LPG + String(" ppm");// Conversion target and combine string ("name" + target + "ppm")
  //String OLEDCO = CO + String(" ppm");
  //String OLEDSMOKE = SMOKE + String(" ppm");
  

  /*display.drawString(0, 0, "CO:");
  display.drawString(0, 32, OLEDCO);
  display.display();
  display.clear();
  delay(1000);
  display.drawString(0, 0, "SMOKE:");
  display.drawString(0, 32, OLEDSMOKE);
  display.display();
  display.clear();
  delay(1000);*/
}

void loop() {
  displayMQ9();
  //Serial.println(ads.readADC_SingleEnded(0));
}
