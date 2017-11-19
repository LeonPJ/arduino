#include <Wire.h>//載入標頭檔
#include <BH1750.h>

BH1750 lightMeter;//將BH1750選告為lightMeter

void setup(){//啟用或reset時觸發
  Serial.begin(9600);//設定baud為9600
  lightMeter.begin();//開始偵測
  Serial.println("Running...");//顯示Running...(換行)
}

void loop() {//啟動後持續進行迴圈
  uint16_t lux = lightMeter.readLightLevel();//讀取lightMeter.readLightLevel()值，存為lux
  Serial.print("Light: ");//顯示Light:(不換行)
  Serial.print(lux);//輸出lux值
  Serial.println(" lx");
  delay(1000);//延遲1s
}
