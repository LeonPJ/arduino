int sensor = 2;        //定義sensor接脚為2
int val = 0;           //定義變量儲存傳感器的返回值

void setup()
{
   Serial.begin(9600);    //設置傳輸鮑率為9600，這裡要跟軟體設置相一致
   pinMode(sensor, INPUT);        //設定sensor接脚為輸入狀態
}

void loop()
{
   val = digitalRead(sensor);    //讀傳感器信息
   if( val== HIGH)
     //如果檢測到人移動
   {
       Serial.println("有人經過");
   }
     //如果沒有檢測到人移動
   else
   {
    Serial.println("無人或人靜止不動"); 
   }
}
