#include <afstandssensor.h>
AfstandsSensor afstandssensor(D6, D7);// Trig D7; Echo D6

float CM = 0;

void setup () {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop () {
digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    while(afstandssensor.afstandCM() <= 10){
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    }
}
