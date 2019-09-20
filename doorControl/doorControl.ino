#define OutDoorPIN D3
#define InDoorPIN D4

void setup() {
  Serial.begin(9600);
  pinMode(OutDoorPIN, INPUT_PULLUP);
  pinMode(InDoorPIN, INPUT_PULLUP);
}

void loop() {
  int count;
  int OutDoorCount = digitalRead(OutDoorPIN);
  //int InDoorCount = digitalRead(InDoorPIN);
  //Serial.println(InDoorCount);
  Serial.println(OutDoorCount);
  /*
  if(OutDoorCount == LOW) {
    int i = 0;
    uint32_t InsideTime = millis();
    while(InDoorCount != LOW || (millis() - InsideTime) > 5000) {
      if((millis() - InsideTime) > 3000) {
        int i = 1;
      }
    }
    if(i != 1) {
      count++;
      Serial.println(count);
    }else{
      Serial.println("Somebody stuck in door");
    }
  }*/

}
