#define Aout A0//MQ-135 AO 接 Arduino Nano A0  
  
int temp = 0;//宣告temp為0
  
void setup() {//啟用或reset時觸發  
  Serial.begin(9600);//設定baud為9600
  pinMode(Aout, INPUT);//定義A0為輸出
}  
  
void loop() {//啟動後持續進行迴圈
  temp = analogRead(Aout);//讀取analogRead(Aout)值存為temp
  Serial.println(temp);//輸出temp的數據 
  delay(500);//延遲0.5s
}  
