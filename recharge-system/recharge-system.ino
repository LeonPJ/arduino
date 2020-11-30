#define batteryPin 32//電池ADC腳位，未設定
#define ACPin 35
#define PVPin 34
#define buckHside 18//PWM
#define buckLside 19//PWM
#define gpioM151 15
#define gpioM141 2
#define gpioM131 16
#define gpioM51 17
#define gpioM101 21
#define gpioM91 3
#define gpioM81 1
#define gpioM71 22
#define gpioM61 30
#define voltageTwevel 3 //12v 電壓轉 3V
#define voltageEight 2//8v 電壓轉 2V
//#define voltageFifteen 15 //15v 電壓
#define voltageTwoPointTwo 2.20//15V 電壓轉2.20
//#define voltageThreePointSix 3.05//22V 電壓轉3.08
#define voltageOne 1
#define currentPointZeroSixtySeven 0.067 //0.067v 電壓
#define voltageTime 1000 //電池電壓偵測時間，目前設定為10 註:1000 微秒 = 1 毫秒，1000000 微秒 = 1 秒
#define offsetVoltage 2.5 //ACS712 0A時輸出2.5v

//#define dutyCycle 255

uint32_t voltageTemporTime;
int count;
float batteryVoltage, batteryVoltageTrans, returnBatteryVoltageFunc, PVVoltage, ACVoltage, batteryVoltageAverage, ACVoltageAverage, PVVoltageAverage, ACVoltageTrans, PVVoltageTrans;
int batteryVoltageValue, batteryVoltageSum, ACVoltageValue, PVVoltageValue, ACVoltageSum, PVVoltageSum;
int batteryState, returnBatteryState, PVState, retrunPVState;

// setting PWM properties
const int freq = 50000;// 設定頻率
const int ledChannel = 0;
const int resolution = 12;
const int dutyCycle = 596;

float batteryVoltageFunc(){
  batteryVoltageSum = 0;
  for(count = 0; count<=5; count++){// 取5次資料
    batteryVoltageValue = analogRead(batteryPin);
    batteryVoltageSum += batteryVoltageValue;
  }
  batteryVoltageAverage = batteryVoltageSum/count;// 平均5次
  batteryVoltageTrans = ((batteryVoltageAverage * 3.3)/4095);
  return batteryVoltageTrans;
}

int batteryJudge(float getBatteryVoltage){
  if(getBatteryVoltage > voltageTwevel){// 如果大於8v 小於 12V
    batteryState = 0;// 條件成立
    }else if(voltageEight < getBatteryVoltage < voltageTwevel){// 如果大於 12V
      batteryState = 1;// 條件不成立
      }else{
        // none
      }
  return batteryState;
}

float ACandPVMeasure(){
  voltageTemporTime = micros();
  ACVoltageSum = 0;
  PVVoltageSum = 0;
  for(count = 0; count<=5; count++){// 取5次資料
    delayMicroseconds(57);
    ACVoltageValue = analogRead(ACPin);
    delayMicroseconds(57);
    PVVoltageValue = analogRead(PVPin);
    ACVoltageSum += ACVoltageValue;
    PVVoltageSum += PVVoltageValue;
  }
  ACVoltageAverage = ACVoltageSum/5; //ADC取樣平均5次
  PVVoltageAverage = PVVoltageSum/5; //ADC取樣平均5次
  ACVoltageTrans = ((ACVoltageAverage * 3.3)/4095);
  PVVoltageTrans = ((PVVoltageAverage * 3.3)/4095);
  //Serial.println(PVVoltageTrans);
  if(PVVoltageTrans > voltageTwoPointTwo){//判斷太陽能電壓
    //Serial.println("太陽能成立");
    startBuck();
    }else if(PVVoltageTrans < voltageTwoPointTwo){
      //Serial.println("市電成立");
      }else{
        //none
        }
   //Serial.println("----------------------");
  /*Serial.print("市電電壓: ");
  Serial.println(ACVoltageTrans);
  Serial.print("太陽能電壓: ");
  Serial.println(PVVoltageTrans);
  Serial.println("----------------------");*/
  
}

int startBuck(){
  //ledcWrite(ledChannel, dutyCycle);// ledcWrite(腳位, 頻率) PWM 腳位 gpio 19
  //Serial.println("打PWM");
  digitalWrite(gpioM151, LOW);// gpio 15
  digitalWrite(gpioM141, HIGH);// gpio 2
  digitalWrite(gpioM131, LOW);// gpio 16
  digitalWrite(gpioM51, HIGH);// gpio 17
  digitalWrite(gpioM91, HIGH);// gpio 3
  digitalWrite(gpioM81, HIGH);// gpio 1
  digitalWrite(gpioM71, LOW);// gpio 22
  digitalWrite(gpioM101, LOW);// gpio 21
  digitalWrite(gpioM61, HIGH);// gpio 30
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(buckHside, ledChannel);
  pinMode(gpioM151, OUTPUT);
  pinMode(gpioM141, OUTPUT);
  pinMode(gpioM131, OUTPUT);
  pinMode(gpioM51, OUTPUT);
  pinMode(gpioM101, OUTPUT);
  pinMode(gpioM91, OUTPUT);
  pinMode(gpioM81, OUTPUT);
  pinMode(gpioM71, OUTPUT);
  pinMode(gpioM61, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  returnBatteryVoltageFunc = batteryVoltageFunc();//讀取電池電壓
  returnBatteryState = batteryJudge(returnBatteryVoltageFunc);
  
  if(returnBatteryState == 1){
    ACandPVMeasure();
  }else if(returnBatteryState == 0){
    Serial.println("否");
  }else{
  
  }
  //startBuck();
  //delay(1000);
  
  //Serial.println("...");
}
