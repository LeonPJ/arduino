float sensor=A0;//需告sensor為A0
float gas_value;
void setup(){//啟用或reset時觸發  
pinMode(sensor,INPUT);//宣告A0為input腳位
Serial.begin(9600);//設定baud為9600
}

void loop(){//啟動後持續進行迴圈
gas_value=analogRead(sensor);//讀取analogRead(sensor)值存為gas_value
Serial.println(gas_value);//輸出gas_value值
}
