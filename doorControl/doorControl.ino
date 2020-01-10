#define InDoorPIN D3// 內側
#define OutDoorPIN D4// 外側
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(InDoorPIN, INPUT_PULLUP);
  pinMode(OutDoorPIN, INPUT_PULLUP);

}

void loop() {
  //delay(100);
int STATUS;
  if((digitalRead(OutDoorPIN) > digitalRead(InDoorPIN)) && digitalRead(InDoorPIN) == LOW && STATUS != 2 && STATUS == 0){
    STATUS = 1;
    int BerforOutStaus = digitalRead(OutDoorPIN);
    while(digitalRead(InDoorPIN) == LOW && BerforOutStaus == HIGH){
      delay(10);
    }
    count++;
    Serial.println(count);
    delay(1000);
  }else if(digitalRead(OutDoorPIN) < digitalRead(InDoorPIN) && digitalRead(OutDoorPIN) == LOW && STATUS !=1 && STATUS == 0){
    STATUS = 2;
    int BerforInStaus = digitalRead(InDoorPIN);
    while(digitalRead(OutDoorPIN) == LOW && BerforInStaus == HIGH){
      delay(10);
    }
    count--;
    if(count <= 0){
      count = 0;
    }
    Serial.println(count);
    delay(1000);
  }else if(STATUS == 1 || STATUS == 2){
        STATUS = 0;
  }else{
    
  }
  /*delay(1000);
Serial.println(digitalRead(InDoorPIN));
Serial.println(digitalRead(OutDoorPIN));
Serial.println("------------");*/
}
