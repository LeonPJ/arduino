void setup()
{
    Serial.begin(9600);
    pinMode(3,OUTPUT);       //設置腳位的模式為輸出
}

void loop()
{
      char order = Serial.read();
      if(order == 43){//on  +
        digitalWrite(3,HIGH);
        }
     
        else if(order == 45){//off  -
          digitalWrite(3,LOW);
          }
          else{
            }
}

