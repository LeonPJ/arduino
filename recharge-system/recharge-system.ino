#define batteryPin 32
#define ACPin 35
#define PVPin 34
#define IPVPin 33
#define buckHside 18//PWM
#define buckLside 19//PWM
#define gpioM151 15
#define gpioM141 2
#define gpioM131 16
#define gpioM51 17
#define gpioM101 21
#define gpioM91 3
#define gpioM81 5
#define gpioM71 22
#define gpioM61 23
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
int batteryState, returnBatteryState, PVState, retrunPVState, startFlybackState;

// setting PWM properties
const int freqOne = 50000;// 設定頻率

const int ledChannelOne = 0;
const int ledChannelTwo = 1;
const int resolutionOne = 10;
const int resolutionTwo = 10;
const int dutyCycleOne = 512;
const int freqTwo = 50000;
const int dutyCycleTwo = 512;
const int dutyShutdown = 0;

  float mPVVoltageValue;
  float mIVoltageValue;
  
  float mPVVoltageSum;
  float mIVoltageSum;
  
  float mPVVoltageAverage;
  float mIVoltageAverage;
  
  float mPVVoltageTrans;
  float mIVoltageTrans;
  float mIVoltageTransToReal;
  float Pn;
  float beforValue = 0;
  float nowValue = 0;
  int tempState;

  float Power_now = 0, Power_anc = 0;

  float pAndO_now = 0;
  float pAndO_anc = 0;
  int batteryVoltageCheck = 0; //判斷電池電壓

  float startBuckPCurrent = 0;
  float startBuckPPrevious = 0;

float batteryVoltageFunc(){// 電池電壓判斷
  batteryVoltageSum = 0;
  for(count = 0; count<=5; count++){// 取5次資料
    batteryVoltageValue = analogRead(batteryPin);
    batteryVoltageSum += batteryVoltageValue;
  }
  batteryVoltageAverage = batteryVoltageSum/count;// 平均5次
  batteryVoltageTrans = ((batteryVoltageAverage * 3.3)/4095);
  return batteryVoltageTrans;
}

int batteryJudge(float getBatteryVoltage){// 預先判斷太陽能是否足夠供電
  if(getBatteryVoltage > 3.06){
    batteryState = 0;// 條件不成立
  }else if( 1.6 < getBatteryVoltage && getBatteryVoltage <= 3.06){
    batteryState = 1;// 條件成立
  }else{
    batteryState = 2;// 沒有條件
  }
  return batteryState;
}

float judgementACVoltage(){// 市電是否足夠
  int rejudgmentACVoltage;
  ACVoltageSum = 0;
  delayMicroseconds(57);
  ACVoltageValue = analogRead(ACPin);
  ACVoltageTrans = ((ACVoltageValue * 3.3)/4095);
  if( 2.04 < ACVoltageTrans && ACVoltageTrans < 3.1){
    rejudgmentACVoltage = 1;
  }else{
    rejudgmentACVoltage = 0;
  }
  return rejudgmentACVoltage;
}

float ACandPVMeasure(){// 市電&太陽能電壓偵測
  ACVoltageSum = 0;
  PVVoltageSum = 0;
  for(count = 0; count<5; count++){// 取5次資料
    delayMicroseconds(57);
    ACVoltageValue = analogRead(ACPin);
    //Serial.println(ACVoltageValue);
    delayMicroseconds(57);
    PVVoltageValue = analogRead(PVPin);
    //Serial.println(PVVoltageValue);
    ACVoltageSum += ACVoltageValue;
    PVVoltageSum += PVVoltageValue;
  }
  ACVoltageAverage = ACVoltageSum/count;// ADC取樣平均5次
  PVVoltageAverage = PVVoltageSum/count;
  ACVoltageTrans = ((ACVoltageAverage * 3.3)/4095);
  PVVoltageTrans = ((PVVoltageAverage * 3.3)/4095);

  /*判斷太陽能電壓是否足夠*/
  if(PVVoltageTrans >= 0.8 && PVVoltageTrans <= 2.91){// 太陽能 成立
    Serial.println("太陽能成立");
    startBuck();// 啟動降壓轉換器
    }else if(PVVoltageTrans < 0.8 || PVVoltageTrans > 2.91){// 市電 成立
      Serial.println("市電成立");
      //startFlyback();
      }else{
        Serial.println("太陽能與市電 不成立");
        // none
      }
}

int startBuck(){// 啟動降壓轉換器
  ledcWrite(ledChannelOne,dutyCycleOne );// ledcWrite(腳位, 頻率) PWM 腳位 gpio 18
  
  float startBuckPVVoltage = ((((analogRead(PVPin)*3.3)/4095)*14.218)+3.626);
  float startBuckPVCurrent = ((((analogRead(IPVPin)*3.3)/4095)-2.5)*1000/100);
  startBuckPCurrent = (startBuckPVVoltage * startBuckPVCurrent);
  /*Serial.print("startBuckPVVoltage: ");
  Serial.println(startBuckPVVoltage);
  Serial.print("startBuckPVCurrent: ");
  Serial.println(startBuckPVCurrent);
  Serial.print("startBuckP: ");
  Serial.println(startBuckPCurrent);
  Serial.print("------------------------");*/

  if(abs(startBuckPCurrent-startBuckPPrevious) > 0.5){
    Serial.println("功率與上一次不同");
    startBuckPPrevious = startBuckPCurrent;
    
    }else if(abs(startBuckPCurrent-startBuckPPrevious) < 0.5){
      Serial.println("功率與上一次相同");
      }else{
        
      }
  
  
  /*float newDuty = (1024 - (((changeDutyVoltage * (-0.212)) + 1.194) * 1024));
  ledcWrite(ledChannelOne, newDuty);// ledcWrite(腳位, 頻率) PWM 腳位 gpio 18
  
  delayMicroseconds(11);// 1000000 = 1*/
  //ledcWrite(ledChannelTwo, dutyCycleTwo);// ledcWrite(腳位, 頻率) PWM 腳位 gpio 19
  //ledcWrite(ledChannelTwo, dutyShutdown);// ledcWrite(腳位, 頻率) PWM 腳位 gpio 19
  
  digitalWrite(gpioM151, LOW);// gpio 15
  digitalWrite(gpioM141, HIGH);// gpio 2
  digitalWrite(gpioM131, LOW);// gpio 16
  digitalWrite(gpioM51, HIGH);// gpio 17
  digitalWrite(gpioM91, HIGH);// gpio 3
  digitalWrite(gpioM81, HIGH);// gpio 5
  digitalWrite(gpioM71, HIGH);// gpio 22
  digitalWrite(gpioM101, LOW);// gpio 21
  digitalWrite(gpioM61, LOW);// gpio 23
  
  //mersureVpvnIpvn();// 偵測Vpvn 與 Ipvn
}

int startFlyback(){
  ledcWrite(ledChannelOne, 1024);// 1024 = full duty
  ledcWrite(ledChannelTwo, 1024);// 1024 = full duty
  
  digitalWrite(gpioM151, HIGH);// gpio 15
  digitalWrite(gpioM141, LOW);// gpio 2
  digitalWrite(gpioM131, HIGH);// gpio 16
  digitalWrite(gpioM51, LOW);// gpio 17
  digitalWrite(gpioM91, LOW);// gpio 3
  digitalWrite(gpioM81, LOW);// gpio 5
  digitalWrite(gpioM101, HIGH);// gpio 21
  CCmode();
}

int CCmode(){// 定電流模式
  digitalWrite(gpioM61, HIGH);// gpio 23
  digitalWrite(gpioM71, LOW);// gpio 22
  mersureVpvVb();
}

int CVmode(){// 定電壓模式
  digitalWrite(gpioM61, LOW);// gpio 23
  digitalWrite(gpioM71, HIGH);// gpio 22
  mersureVpvVbTwo();
}

float mersureVpvnIpvn(){// 偵測Vpvn 與 Ipvn
  delayMicroseconds(57);
  mPVVoltageValue = analogRead(PVPin);
  delayMicroseconds(57);
  mIVoltageValue = analogRead(IPVPin);
  mPVVoltageTrans = ((mPVVoltageValue * 3.3)/4095);
  mIVoltageTrans = ((mIVoltageValue * 3.3)/4095);
  Power_now = (mPVVoltageTrans * mIVoltageTrans);
  Serial.print("功率: ");
  Serial.println(Power_now);
  if ((Power_now - Power_anc) > 0.05){
    Serial.println("比前一次大");
    P_and_O(mPVVoltageTrans);
  }else if((Power_now - Power_anc) < -0.05){
    Serial.println("比前一次小");
    P_and_O(mPVVoltageTrans);
  }else{
    Serial.println("沒有變化");
  }
  Power_anc = Power_now;
  delay(1000);
}

float P_and_O(float getMPVVoltageTrans){
  //delayMicroseconds(57);
  delay(1000);
  mPVVoltageValue = analogRead(PVPin);
  pAndO_now = ((mPVVoltageValue * 3.3)/4095);
  pAndO_anc = getMPVVoltageTrans;

  /*Serial.print("前次: ");
  Serial.println(pAndO_anc);
  Serial.print("當前: ");
  Serial.println(pAndO_now);
  Serial.println("------------------");*/
  if (pAndO_now > pAndO_anc){
    Serial.println("電壓 比前一次大");
    ledcWrite(ledChannelTwo, dutyCycleTwo);// gpio 19
  }else if(pAndO_now < pAndO_anc){
    Serial.println("電壓 比前一次小");

  }else{
    Serial.println("電壓 沒有變化");
  }
  pAndO_anc = pAndO_now;
  //delay(1000);
}

float mersureVpvVb(){
  delayMicroseconds(57);
  float mersBatteryVoltageValue = analogRead(batteryPin);
  delayMicroseconds(57);
  float mersPVVoltageValue = analogRead(PVPin);
  
  float mersBatteryVoltageTrans = ((mersBatteryVoltageValue * 3.3)/4095);
  float mersPVVoltageTrans = ((mersPVVoltageValue * 3.3)/4095);
  
  PVEnough(mersBatteryVoltageTrans, mersPVVoltageTrans);
}

int PVEnough(float dataBattery, float dataPV){// 太陽能是否足夠
  if(dataPV > 2.08 && dataPV < 3.06){
      startBuck();
      }else{
        if(dataBattery > 3 && dataBattery < 3.2){
          CVmode();
          }else if(dataBattery < 3 && dataBattery > 2){
            CVmode();
            }else{
              // none
            }
      }
}

//------------------------------------------
float mersureVpvVbTwo(){
  delayMicroseconds(57);
  float mersBatteryVoltageValueTwo = analogRead(batteryPin);
  delayMicroseconds(57);
  float mersPVVoltageValueTwo = analogRead(PVPin);
  
  float mersBatteryVoltageTransTwo = ((mersBatteryVoltageValueTwo * 3.3)/4095);
  float mersPVVoltageTransTwo = ((mersPVVoltageValueTwo * 3.3)/4095);
  
  PVEnoughTwo(mersBatteryVoltageTransTwo, mersPVVoltageTransTwo);
}

int PVEnoughTwo(float dataBatteryTwo, float dataPVTwo){// 太陽能是否足夠
  if(dataPVTwo > 2.08 && dataPVTwo < 3.06){
      startBuck();
      }else{
        // 
      }
}
//------------------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ledcSetup(ledChannelOne, freqOne, resolutionOne);
  ledcAttachPin(buckHside, ledChannelOne);
  
  ledcSetup(ledChannelTwo, freqTwo, resolutionTwo);
  ledcAttachPin(buckLside, ledChannelTwo);
  
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
  
  ///returnBatteryVoltageFunc = batteryVoltageFunc();// 讀取電池電壓
  //returnBatteryState = batteryJudge(returnBatteryVoltageFunc);// 成立回傳1，不成立回傳0
  
  /*電池電壓判斷*/
  /*if(returnBatteryVoltageFunc > 1.6 && returnBatteryVoltageFunc <= 3.06){// Vb < 8.4v , > 5v
    Serial.println("電池電壓 小於 8.4v, 大於 5v");
    batteryVoltageCheck = 1;
  }else if(returnBatteryVoltageFunc < 1.6){// 沒有條件
    Serial.println("電池電壓 未量測到");
    batteryVoltageCheck = 2;
  }else{
    Serial.println("電池電壓 大於 8.4v");
    batteryVoltageCheck = 3;
  }*/

  ///if(batteryVoltageCheck == 1){
    ACandPVMeasure();// 市電&太陽能電壓偵測
  ///}
  
  //ledcWrite(ledChannelOne, dutyCycleTwo);// gpio 18
  //else if(returnBatteryState == 0){
    //if(judgementACVoltage() == 1){
      //Serial.println("大於喔!");
      //startFlyback();
    //}else if(judgementACVoltage() == 0){
      //Serial.println("小於喔!");
      // 未寫
    //}else{
      //none
    //}
  //}else{
  
  //}


  //startBuck();
  //mersureVpvnIpvn();
  delay(500);
  
  //Serial.println("...");
}
