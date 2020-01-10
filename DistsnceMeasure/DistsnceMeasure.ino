#include <afstandssensor.h>
AfstandsSensor afstandssensor(D7, D6);// Trig D7; Echo D6

float CM = 0;

void setup () {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
}

void loop () {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    CM = afstandssensor.afstandCM();
    while(CM <= 10){
      digitalWrite(D1, HIGH);
      delay(100);
      digitalWrite(D2, HIGH);
      delay(100);
      digitalWrite(D3, HIGH);
      delay(100);
      digitalWrite(D1, LOW);
      delay(100);
      digitalWrite(D2, LOW);
      delay(100);
      digitalWrite(D3, LOW);
      delay(100);
    }
      digitalWrite(D1, LOW);
      delay(100);
      digitalWrite(D2, LOW);
      delay(100);
      digitalWrite(D3, LOW);
      delay(100);
    /*if(CM <= 10){
      Serial.println("lower 5 cm");
      digitalWrite(D1, HIGH);
      delay(100);
      digitalWrite(D2, HIGH);
      delay(100);
      digitalWrite(D3, HIGH);
      delay(100);
      digitalWrite(D1, LOW);
      delay(100);
      digitalWrite(D2, LOW);
      delay(100);
      digitalWrite(D3, LOW);
      delay(100);
    }else{
      digitalWrite(D1, LOW);
      delay(100);
      digitalWrite(D2, LOW);
      delay(100);
      digitalWrite(D3, LOW);
      delay(100);
    }*/
}
