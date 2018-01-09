String code = "";           //initialize the output string
boolean endbit = 0;            //a flag to mark 0D received
char temp;
int Led=13;     
int buttonpin=3;
int val;
void setup() {
  Serial.begin(9600);       //initialize the Serial port
  pinMode(Led,OUTPUT);     //pin13 
  pinMode(buttonpin,INPUT);//limit switch a contact
}

void loop() {
  val=digitalRead(buttonpin);
  if(val==HIGH) { 
  digitalWrite(Led,HIGH);
  }
  /*else { 
  digitalWrite(Led,LOW);
  }*/

  if (Serial.available() > 0) {
    temp = char( Serial.read());    //read the input data
    code += temp;
    digitalWrite(Led,LOW);
  }
  if (temp == 0x0D) {           // Or temp == '\r'
    Serial.println(code);
    code = "";
    endbit = 0;
    temp = 0;
    digitalWrite(Led,LOW);
  }
}
