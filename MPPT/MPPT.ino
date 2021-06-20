#define PWM 18 // PWM 腳位
const int freq = 50000;// 頻率   const 是常數，設定之後，不可更改
const int ledChannel = 0;// ledChannel 0
const int resolution = 10;// 解析度10 -> 2的10次方
const int changeD = 20.48;// change duty by 2% 
float initDutyCycle = 75;// duty 初始數值
float Vnow = 0;// 目前電壓
float Vex = 0;// 前次電壓
float Vdiff = 0 ;// 目前電壓 - 前次電壓
float I = 0;// 目前電流
float Pnow = 0;// 目前功率 
float Pex = 0;// 前次功率
float Pdiff = 0 ;// 目前功率 - 前次功率
int PVtimes = 0;// 太陽能初始計數
int preventJump;// 防止功率跳

//int PdiffStatus;
//float offsetVoltage = 1.12;

int initPVtime(){// 回傳進入計數次數
    return PVtimes++;// 回傳至呼叫處
  }

void setup() {  
  ledcSetup(ledChannel, freq, resolution);// (頻道, 頻率, 解析度)
  ledcAttachPin(PWM, ledChannel);//　(輸出腳位,　頻道)
  //Serial.begin(9600); //boudrate 設定，要看println 時要打開，其他程式中的print 或是println 註解也要拿掉
  pinMode(34, INPUT);// Vpv
  pinMode(33, INPUT);// Ipv
}

void loop() {
  int VADC = analogRead(34);
  delay(20); // 判讀時不可 邊讀 邊取資料，不然讀出來的資料會是錯誤，所以要加delay，單位sm，delay(1000) -> 1s
  int IADC = analogRead(33);
  
  Vnow = VADC * (3.3 / 4095.0); 
  I = IADC * (3.3/ 4095.0); 
  //Serial.print("v: ");
  //Serial.println(Vnow);
  //Serial.print("i: ");
  //Serial.println(I);
  
  Pnow = Vnow * I;

  int getinitPVtime = initPVtime();// 將回傳數值，儲存至getinitPVtime

  if(getinitPVtime > 1){// 程式執行次數大於1 時才會進入比較Pdiff, Vdiff 與設定ledcWrite
    Pdiff = Pnow - Pex;
    Vdiff = Vnow - Vex;
    ledcWrite(ledChannel, initDutyCycle);
  }else{
    // none
  }

  if(abs(Pdiff) < 0.05){
    preventJump = 1;// 不修改initDutyCycle
  }else if(abs(Pdiff) > 0.05){
    preventJump = 0;// 修改initDutyCycle
  }else{
    // none
  }

  if(preventJump == 0){
    if(Vdiff < 0){// ( Pnow - Pex > 0 ) + ( Vnow - Vex < 0 )
      //Serial.println("Pdiff < 0.00 --> Vdiff < 0 --> initDutyCycle - changeD");
      initDutyCycle = initDutyCycle - changeD;
    }else if(Vdiff > 0){// ( Pnow - Pex > 0 ) + ( Vnow - Vex < 0 )
      //Serial.println("Pdiff < 0.00 --> Vdiff < 0 -->  initDutyCycle + changeD");
      initDutyCycle = initDutyCycle + changeD;
    }else if(Pdiff > 0.00 && preventJump == 0){
      if(Vdiff < 0){// ( Pnow - Pex > 0 ) + ( Vnow - Vex < 0 )
        //Serial.println("Pdiff > 0.00 --> Vdiff < 0 --> initDutyCycle + changeD");
        initDutyCycle = initDutyCycle + changeD; 
      }else if(Vdiff > 0){// ( Pnow - Pex > 0 ) + ( Vnow - Vex < 0 )
        //Serial.println("Pdiff > 0.00 --> Vdiff > 0 --> initDutyCycle - changeD");
        initDutyCycle = initDutyCycle - changeD;
      }
      }else if(preventJump == 1){
      initDutyCycle = initDutyCycle;
    }else{
    // none
    }
  }
 
  if(initDutyCycle < 75){// 防止initDutyCycle 小於75 或是大於500
    initDutyCycle = 75;
  }else if(initDutyCycle > 500){
    initDutyCycle = 500;
  }else{
    //none
  }
  
  //Serial.print("initDutyCycle: ");
  //Serial.println(initDutyCycle);
  
  Pex = Pnow;
  Vex = Vnow;
  delay(500);// 最好(至少)有0.2s 的delay，程式執行時才不會當掉
  //Serial.println("-------------");
}
