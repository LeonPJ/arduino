int val;// 設定變量val
void setup() {
  pinMode(D5, INPUT);// 設定感測器为输入
  Serial.begin(9600);
}

void loop()
{
val = digitalRead(D5);// 將感測器的值讀取给val
//Serial.println(val);

if(val == HIGH){ // 當感測器檢測有信號时，LED 閃爍
  Serial.println("無火");
  }
  else {
    Serial.println("火苗出現");
    }

  }

