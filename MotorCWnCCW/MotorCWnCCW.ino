void setup()
{
  Serial.begin(9600);
  pinMode(3,OUTPUT);       //D3 output
  pinMode(4,OUTPUT);       //D4
  digitalWrite(3,HIGH);    
  digitalWrite(4,HIGH);
}

void loop() 
{
  if (Serial.available() > 0)// 檢查是否有資料可供讀取
    {
    char order = Serial.read();
    switch(order)
      {
      case 43://CW '+'
        digitalWrite(3,HIGH);
        digitalWrite(4,LOW);
        break;

      case 45://CCW '-'
        digitalWrite(3,LOW);
        digitalWrite(4,HIGH);
        break;
        

      case 10://new line ASCII
        break;

      case 13://carriage return ASCII
        break;
        
      default:
        Serial.println("Enter Error");
        digitalWrite(3,HIGH);
        digitalWrite(4,HIGH);
        break;
      }
  }
}
