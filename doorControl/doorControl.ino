#define InDoorPIN D3
#define OutDoorPIN D4
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(OutDoorPIN, INPUT_PULLUP);
  pinMode(InDoorPIN, INPUT_PULLUP);
}

void loop() {
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

}
