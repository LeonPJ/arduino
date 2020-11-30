// the number of the LED pin
const int ledPin = 16; //GPIO 16 接示波器

// setting PWM properties
const int freq = 50000;// 設定頻率
const int ledChannel = 0;
const int resolution = 8;

uint32_t stateTime;

int count;

#define hold 2000// 保持2秒

void PWNFunc(){
    // increase the LED brightness 
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, 127);// ledcWrite(腳位, 頻率)
}

void OtherFunc(){
  stateTime = millis();
  count = 0;
  while((millis() - stateTime) < hold){ //電流電壓
    count++;
    //delay(500); //如果執行程式會崩潰時，再將前面註解拿掉
  }
}

void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  Serial.begin(9600);
}
 
void loop(){
  PWNFunc();// 執行PWM 副程式

  //OtherFunc();// OtherFunc 副程式內回有迴圈去做沒有意義呃事情，查看GPIO 16腳位輸出頻率(f)是否是5000
  
  Serial.println("...");
  delay(500);
}
