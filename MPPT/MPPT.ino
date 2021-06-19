/*
 * MPPT converter control code
  ReadAnalogVoltage
  Reads an analog input on pin A3, converts it to voltage, and prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A3, and the outside pins to +3V and ground.
  
  Hardware Required:
  * MSP-EXPF5529 LaunchPad
  * Voltage Divider ccts
 
  This example code is in the public domain.
*/


/*const float M= 13; // ~0.05 DC variation
float V1;
float exV; 
float exI;
float delV;*/

#define PWM 18

const int freq = 50000;
const int ledChannel = 0;// The channel that generates the signal is the ledChannel
const int resolution = 10;
float initDutyCycle = 75;

float Vnow = 0;// 目前電壓
float Vex = 0;// 前次電壓
float Vdiff = 0 ;// 目前電壓 - 前次電壓

float I = 0;// 目前電流

float Pnow = 0;// 目前功率 
float Pex = 0;// 前次功率
float Pdiff = 0 ;// 目前功率 - 前次功率
float Vref;
float delVref;
//int PWM= A1;// external LED attached
int dc=127; // 50% DC assumed initially

int PVtimes = 0;// 太陽能初始計數

int PdiffStatus;

float offsetVoltage = 1.12;

int initPVtime(){
    return PVtimes++;
  }


// the setup routine runs once when you press reset:
void setup() {
  //V1= 0;
  //exV=0;
  //exI=0;
  //I= 0;
  //exP= 2.5; 
  //Pnow= 0;
  
  ledcSetup(ledChannel, freq, resolution);// (頻道, 頻率, 解析度)
  ledcAttachPin(PWM, ledChannel);//　(輸出腳位,　頻道)

  delVref= 0; // probs right
  Vref= 1.5;    //cheggit
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); // msp430g2231 must use 4800
  pinMode(34, INPUT);// Vpv
  pinMode(33, INPUT);// Ipv
  //pinMode(PWM, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int VADC = analogRead(34);
  delay(20); // 20ms delay between sampling 判讀時不可 邊讀 邊取資料，不然讀出來的資料會是錯誤，所以要加delay
  int IADC = analogRead(33);
  
  Vnow = VADC * (3.3 / 4095.0); 
  I = IADC * (3.3/ 4095.0); 
  Serial.print("v: ");
  Serial.println(Vnow);
  Serial.print("i: ");
  Serial.println(I);
  
  //constrain (voltage, 0.1, 3.3);
  //constrain (current, 0.1, 3.3);
  
  Pnow = Vnow * I;
  Serial.print("Pnow: ");
  Serial.println(Pnow);
  //Serial.println("-------------");

  int getinitPVtime = initPVtime();

  Serial.print("getinitPVtime: ");
  Serial.println(getinitPVtime);
  
  if(getinitPVtime > 1){
    Pdiff = Pnow - Pex;
    Vdiff = Vnow - Vex;
    //Serial.print("Look Here Vdiff: ");
    //Serial.println(Vdiff);
    ledcWrite(ledChannel, initDutyCycle);
    Serial.print("initDutyCycle: ");
    Serial.println(initDutyCycle);
    //Serial.println("*************************************************************************************************");
  }else{

  }
  
  Pex = Pnow;
  Vex = Vnow;
  
  Serial.print("Newest Pex: ");
  Serial.println(Pnow);

  if(Pdiff < 0.00){
    PdiffStatus = 0;
    Serial.print("PdiffStatus: ");
    Serial.println(PdiffStatus);
  }else if(Pdiff > 0.00){
    PdiffStatus = 1;
    Serial.print("PdiffStatus: ");
    Serial.println(PdiffStatus);
  }
  
  if(Pdiff < 0.00){// Pnow - Pex < 0
    //Serial.println("p low");
    Serial.print("current Vdiff: ");
    Serial.println(Vdiff);
    if(Vdiff < 0){// ( Pnow - Pex < 0 ) + ( Vnow - Vex < 0 )
      Serial.println("p low and v low");
      initDutyCycle--;
      initDutyCycle--;
      initDutyCycle--;
      initDutyCycle--;
      if(initDutyCycle < 75){
        initDutyCycle = 75;
        }else if(initDutyCycle > 410){
          initDutyCycle = 410;
        }else{
          // 不做任何事
      }
      ledcWrite(ledChannel, initDutyCycle);
      Serial.print("initDutyCycle: ");
      Serial.println(initDutyCycle);
    }/*else if(Vdiff > 0){// ( Pnow - Pex < 0 ) + ( Vnow - Vex > 0 )
      Serial.println("p low and v high");
      initDutyCycle++;
      initDutyCycle++;
      initDutyCycle++;
      initDutyCycle++;
      if(initDutyCycle < 75){
        initDutyCycle = 75;
        }else if(initDutyCycle > 410){
          initDutyCycle = 410;
        }else{
          // 不做任何事
      }
      ledcWrite(ledChannel, initDutyCycle);
      Serial.print("initDutyCycle: ");
      Serial.println(initDutyCycle);
    }*/
  }
  
  if(Pdiff > 0){
    //Serial.println("p high");
    /*if(Vdiff < 0.00){
      Serial.println("p high and v low");
      initDutyCycle++;
      initDutyCycle++;
      initDutyCycle++;
      initDutyCycle++;
      if(initDutyCycle < 75){
        initDutyCycle = 75;
        }else if(initDutyCycle > 410){
          initDutyCycle = 410;
        }else{
          // 不做任何事
      }
      ledcWrite(ledChannel, initDutyCycle);
      Serial.print("initDutyCycle: ");
      Serial.println(initDutyCycle);
    }else */if(Vdiff > 0){
      Serial.println("p high and v high");
      initDutyCycle--;
      initDutyCycle--;
      initDutyCycle--;
      initDutyCycle--;
      if(initDutyCycle < 75){
        initDutyCycle = 75;
        }else if(initDutyCycle > 410){
          initDutyCycle = 410;
        }else{
          // 不做任何事
      }
      ledcWrite(ledChannel, initDutyCycle);
      Serial.print("initDutyCycle: ");
      Serial.println(initDutyCycle);
    }
  }
  
  delay(500);
  Serial.println("-------------");
 /*
 float delP= Pnow- exP;
 delV = voltage- exV;
 delVref= M * (delP/ voltage- exV);
 //Vref= Vref+- delVref; 
 
 if (delP > 0){

if (delV > 0)
      Vref= Vref + delVref;
    
else
      Vref = Vref - delVref;
 }


 else 
{
if (delV < 0)
      Vref= Vref + delVref;
    
else
      Vref = Vref - delVref;
 }

//modulating duty cycle
  dc= dc + (Vref * M);
constrain (dc, 0, 255);
analogWrite(PWM, dc);
    
  // print out the value you read:
  Serial.println("\n");
  Serial.println(delP );
  Serial.println("DUTY CYCLE : \n" );
  Serial.println((dc+1)/256* 100);

  exV= voltage;
  exI= current;
  exP= exV* exI; 
  
  delay(100);*/
  
}
