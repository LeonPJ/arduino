#define Aout A0 //MQ-135 AO 接 Arduino Nano A0  
  
int temp = 0; //存取A0讀取的數據
  
void setup() {  
  // put your setup code here, to run once:  
  Serial.begin(9600);//Baud
  pinMode(Aout, INPUT);//定義A0為輸出
}  
  
void loop() {  
  // put your main code here, to run repeatedly:  
  temp = analogRead(Aout); //讀取A0的模擬數據
  Serial.println(temp); //輸出temp的數據 
  delay(500);  //延遲0.5s  
}  
