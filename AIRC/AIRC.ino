/*
  Arduino Infrared Remote Control(AIRC).
  Written by Charles Law
  Version 2.0
  Feb 2016
  
  AIRC is based on:
  "A Multi-Protocol Infrared Remote Library for the Arduino"
  Written by Ken Shirriff
  Version 2.0.1 June, 2015
  http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html
  https://github.com/z3t0/Arduino-IRremote
  
  AIRC accepts the following commands:
    Status
    RxEnable
    RxDisable
    TxHide
    TxShow
    PLHide
    PLShow
    PLGap
    PLUnknown
    ReadPulses
    Pulses
    NEC
    SharpRaw
    Sharp
    Panasonic
    Sony
    
  Commands and corresponding parameters are separated by space or tab,
  and they are not case sensitive.
  
  Bin, Dec, Hex based number can be used for all numeric parameters.
  Default is Dec, while Bin or Hex based number can be used with a leading "b"
  or "h" respectively, eg. b1001, h7f.
  
  Hyphen and comma can be added anywhere within a numeric parameter for
  clarity. They are ignored when read by AIRC.
  eg. b1000-11-11 = b10001111, 6,5,535 = 65535, hab-cd,ef = habcdef
  
  "m" or "s" can also be added for clarity, especially when used for denoting
  mark or space pulse length. They are ignored when read by AIRC.
  
  Details for each command and corresponding parameters are described as
  follows:
  
  Status
  Print Rx, Tx and PL status.
  
  RxEnable
  Default. Enable receiver.
  
  RxDisable
  Disable receiver.
  
  TxHide
  Default. Hide transmit message.
  
  TxShow
  Show transmit message.
  
  PLUnknown
  Default. Shows received pulse length only if un-decoded.
  
  PLGap
  Shows previous gap length only.
  
  PLHide
  Always hide received pulse length.
  
  PLShow
  Always show received pulse length.
  
  ReadPulses Gap
  Read incoming IR stream pulses length.
    Gap....min gap length, in us, between each IR streams.
           max = 1000000us(1s).
           default = _GAP if omitted, that is defined in IRremoteInt.h
           max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
           while RAWBUF is defined in IRremoteInt.h, same for Pulses.
  
  Pulses kHz InitialEnd Repeat m1 s2 m3 s4 m5 s6....sX
  Transmit and decode in pulse length format.
    kHz             int, Carrier frequency in kHz.
                    According to IRsend::enableIROut from irSend.cpp, it is
                    designed for 36~40kHz.
                    Decodes only if kHz=0.
    InitialEnd      int, End index of initial stream, including the gap space.
                    default = total no. of raw values if 0 is entered.
    Repeat          int, No. of repeat.
                    Repetitive IR stream will be send indefinitely if
                    Repeat >= 1000.
                    Repeat can be terminated when new TxCode command line is
                    sent to AIRC, same for other transmit commands.
    m1 s2...sX      Pulse length of each mark/space, in us, starting with the
                    the first mark. An extra space length of 1s will be added
                    if the final pulse length is a mark.
                    max. pulse length is 1000000us(1s).
                    max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
                    while RAWBUF is defined in IRremoteInt.h
  
  NEC Data Nbits Repeat
  Transmit NEC Protocol
    Data      unsigned long
    Nbits     int, optional, max = 32, default = 32 if omitted
    Repeat    int, optional, default = 0 if omitted or Nbits is
              omitted
    
  SharpRaw Data Nbits Repeat
  Transmit Sharp protocol in raw format
    Data      unsigned long
    Nbits     int, optional, default = 15 if omitted
    Repeat    int, optional, default = 0 if omitted or Nbits is omitted
  
  Sharp Address Command Repeat
  Transmit Sharp protocol in address and command format
    Address    unsigned int
    Command    int
    Repeat     int, optional, default = 0 if omitted
  
  Panasonic Address Data Repeat
  Transmit Panasonic protocol
    Address    unsigned int
    Data       unsigned long
    Repeat     int, optional, default = 0 if omitted
  
  Sony Data Nbits Repeat
  Transmit Sony protocol
    Data      unsigned long
    Nbits     int
    Repeat    int, optional, default = 0 if omitted

*/

#include <IRremote.h>
#include <IRremoteInt.h>

#define IRLedGnd 4           // IR LED Gnd pin
#define VccPin 5             // IR receiver VccPin
#define GndPin 6             // IR receiver Gnd pin
#define RECV_PIN 7           // IR receiver data pin

IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend irsend;

String CMD;                  // AIRC command
int Repeat;
unsigned long Gap;           // Gap length in us
int kHz;
int RawLen;

unsigned long TimeStart;
unsigned long TimeEnd;
unsigned long TimeQuit;
int IndexStart;
int IndexEnd;
int InitialEnd;
char ThisChar;
char NextChar;
int BufMaxIndex;
boolean myError;
boolean TxShow;
boolean RxEnable;
boolean RepeatAbort;
byte PLShow;                 // hide or show pulse length
                             // 0=hide all, 1=shows gap only
                             // 2=shows all if un-decoded(default), 3=shows all
unsigned long myULTemp;
unsigned long myBaud;
byte bdh;                    // bin = 2, dec = 10, hex = 16
int NumParaLen;              // no. of numeric command line parameters
unsigned long NumPara[3];    // Numeric command line parameters.
                             // Array size of 3 is enough for all send commands
                             // available from the current library.
                             // Usually 2 for send command plus 1 for repeat.

void setup() {
  myBaud = 9600;
  Serial.begin(myBaud);
  delay(100);
  
  pinMode(IRLedGnd, OUTPUT);      // Gnd for IR LED
  digitalWrite(IRLedGnd, LOW);
  pinMode(GndPin, OUTPUT);        // Gnd for IR Receiver
  digitalWrite(GndPin, LOW);
  pinMode(VccPin, OUTPUT);        // Vcc for IR Receiver
  digitalWrite(VccPin, HIGH);
  
  pinMode(9, OUTPUT);             // Gnd for extra LED
  digitalWrite(9, LOW);
  pinMode(8, OUTPUT);             // anode of extra LED
  digitalWrite(8, LOW);
  
  irrecv.enableIRIn();            // Start the receiver
  irrecv.blink13(1);              // Blink onboard LED while receiving or transmitting
  TxShow = false;                 // Default, hide TxMessage
  PLShow = 2;                     // Default, shows Pulse length only if un-decoded
  RxEnable = true;                // Default, enable receive
  Repeat = 0;
  NextChar = -1;
  irparams.rawbuf[0] = 65535;
  BufMaxIndex = RAWBUF - 1;
  if (BufMaxIndex % 2) BufMaxIndex--;
}

void loop() {
  // Check IR in
  if (irrecv.decode(&results)) {
    Serial.println("Rx:");
    PrintDecodedResult(&results);
    irrecv.resume();              // Receive the next value
    Serial.println("");
  }
  
  // Check Serial in
  ThisChar = 255;
  if (Serial.available()) {
    myError = false;
    NextChar = Serial.peek();
    SerialSkip();                 // skip space, tab, NL if any.
    if (ThisChar == 10) Repeat = 0;    // reset Repeat
    else if (ThisChar != 10) {         // not an empty line
      disableIRIn();
      CMD = ReadCMD();
      if      (CMD.equalsIgnoreCase("ReadPulses"))  ReadPulses();
      else if (CMD.equalsIgnoreCase("Pulses"))      TxPulses();
      else if (CMD.equalsIgnoreCase("NEC"))         TxCode();
      else if (CMD.equalsIgnoreCase("SharpRaw"))    TxCode();
      else if (CMD.equalsIgnoreCase("Sharp"))       TxCode();
      else if (CMD.equalsIgnoreCase("Panasonic"))   TxCode();
      else if (CMD.equalsIgnoreCase("Sony"))        TxCode();
      /*      // for other send command
      else if (CMD.equalsIgnoreCase("XXXX"))        TxCode();
      */
      else {       // for commands without numeric parameter
        if (ThisChar != 10) SerialInClear();     // clear the rest of the current line
        if      (CMD.equalsIgnoreCase("Status"))    PrintStatus();
        else if (CMD.equalsIgnoreCase("RxEnable"))  RxEnable = true;
        else if (CMD.equalsIgnoreCase("RxDisable")) RxEnable = false;
        else if (CMD.equalsIgnoreCase("TxShow"))    TxShow   = true;
        else if (CMD.equalsIgnoreCase("TxHide"))    TxShow   = false;
        else if (CMD.equalsIgnoreCase("PLHide"))    PLShow   = 0;
        else if (CMD.equalsIgnoreCase("PLGap"))     PLShow   = 1;
        else if (CMD.equalsIgnoreCase("PLUnknown")) PLShow   = 2;
        else if (CMD.equalsIgnoreCase("PLShow"))    PLShow   = 3;
        else Serial.println(CMD + ", invalid command." + '\n');
      }
    }
    if ((RxEnable) && (Serial.available() ==0)) irrecv.enableIRIn();
    else disableIRIn();
  }
  else Repeat = 0;
}

void TxPulses() {            // Transmit and decode in pulse length format.
  /*
  Pulses kHz InitialEnd Repeat m1 s2 m3 s4 m5 s6....sX
    kHz             int, Carrier frequency in kHz.
                    According to IRsend::enableIROut from irSend.cpp, it is
                    designed for 36~40kHz.
                    Decodes only if kHz=0.
    InitialEnd      int, End index of initial stream, including the gap space.
                    default = total no. of raw values if 0 is entered.
    Repeat          int, No. of repeat.
                    Repetitive IR stream will be send indefinitely if
                    Repeat >= 1000.
                    Repeat can be terminated when new command line is sent to 
                    AIRC, same for other transmit commands.
    m1 s2...sX      Pulse length of each mark/space, in us, starting with the
                    the first mark. An extra space length of 1s will be added
                    if the final pulse length is a mark.
                    max. pulse length is 1000000us(1s).
                    max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
                    while RAWBUF is defined in IRremoteInt.h 
  */
  
  if (Repeat != 0) {
    Serial.println("Please re-entry the Pulses command line.");
    Serial.println("");
    SerialInClear();
    Repeat = 0;
    return;
  }
  
  disableIRIn();
  
  int IndexStart = 0;
  int IndexEnd = 0;
  int FixOne = 58;
  int FixTwo = 100;
  myError = false;
  irparams.rawbuf[0] = 20000;     // 1s in ticks
  
  // read kHz
  if (ThisChar != 10) kHz = ReadOneValue();
  else myError = true;
  
  // read InitialEnd
  if (ThisChar != 10) InitialEnd = ReadOneValue();
  else myError = true;
  if (InitialEnd % 2) InitialEnd++;     // must end with space, ie even no.
  
  // read Repeat
  if (ThisChar != 10) Repeat = ReadOneValue();
  else myError = true;
  
  // read pulse lengths
  RawLen = 0;
  while ((!myError) && (ThisChar != 10) && (RawLen < BufMaxIndex -1)) {
    myULTemp = ReadOneValue();
    if (!myError) {
      RawLen++;
      if (myULTemp > 1000000) myULTemp = 1000000;
      irparams.rawbuf[RawLen] = (unsigned int) BufTime2Raw(myULTemp);
    }
  }
  if (ThisChar != 10) SerialInClear();
  if (myError) {
    PrintError();
    return;
  }
  // add 1s space if ends with a mark
  if ((RawLen %2) && (RawLen < BufMaxIndex)) {
    RawLen++;
    irparams.rawbuf[RawLen] = 65535;
  }
  
  // change InitialEnd if necessary
  if ((InitialEnd > RawLen) || (InitialEnd == 0)) InitialEnd = RawLen;
  
  // print command line
  if ((TxShow) || (kHz == 0)){
    if (kHz == 0) Serial.println("Rx:");
    else Serial.println("Tx:");
    Serial.print("Pulses");
    PrintLeadingSpace(kHz, 3);
    Serial.print(kHz, DEC);
    PrintLeadingSpace(InitialEnd, 5);
    Serial.print(InitialEnd, DEC);
    PrintLeadingSpace(Repeat, 6);
    Serial.print(Repeat, DEC);
    PrintPulsesLength(1, RawLen, 1);
    Serial.println("");
  }
  
  
  if (kHz > 0) {        // transmit
    BLINKLED_ON();
    irsend.enableIROut(kHz);
    IndexStart = 1;
    IndexEnd = InitialEnd;
    Repeat++;           // add 1 for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      for (int i = IndexStart; i <= IndexEnd; i++){
        myULTemp = BufRaw2Time(irparams.rawbuf[i]);
        if (i % 2) {              // mark
          TIMER_ENABLE_PWM;
          if (myULTemp > FixOne + FixTwo) myULTemp = myULTemp - FixOne -FixTwo;
          else myULTemp = 0;
        }
        else {                   // space
          TIMER_DISABLE_PWM;
          if (myULTemp > FixTwo - FixOne) myULTemp = myULTemp - FixOne + FixTwo;
          else myULTemp = 0;
        }
        delayMicroseconds(myULTemp % 1000);
        if (myULTemp >= 1000) delay(myULTemp / 1000);        
      }
      if (InitialEnd != RawLen) {
        IndexStart = InitialEnd +1;
        IndexEnd = RawLen;
      }
      if ((Repeat > 0) && (Repeat <= 1001)) Repeat--;
    }
    TIMER_DISABLE_PWM;       // make sure IR LED is off
    BLINKLED_OFF();
    if (Repeat > 0) Serial.println("Repeats aborted.");
    if (TxShow) Serial.println("TxDone.");
  }
  
  // try to decode
  if ((kHz == 0) || (TxShow)) {
    IndexStart = 1;
    while (IndexStart <= RawLen) {
      if (kHz == 0) Serial.println("RxPulses:");
      else Serial.println("TxPulses:");
      Serial.print("Pulses");
      PrintLeadingSpace(kHz, 3);
      Serial.print(kHz, DEC);
      IndexEnd = IndexStart;
      
      // look for a gap
      while ((IndexEnd < RawLen) && 
      ((BufRaw2Time(irparams.rawbuf[IndexEnd]) < _GAP) || (IndexEnd % 2))) IndexEnd++;
      
      irparams.rawlen = (unsigned int) (IndexEnd - IndexStart +1);
      PrintLeadingSpace(irparams.rawlen, 5);
      Serial.print(irparams.rawlen, DEC);
      Serial.print("     0");
      PrintPulsesLength(IndexStart, IndexEnd, 1);
      Serial.println("");
      
      // convert buffer values to ticks and shift index
      for (int i = IndexStart; i <= IndexEnd; i++) {
        irparams.rawbuf[i - IndexStart +1] = BufRaw2Time(irparams.rawbuf[i]) / USECPERTICK;
      }
      
      irparams.rcvstate = STATE_STOP;
      if (kHz == 0) Serial.println("RxCode: ");
      else Serial.println("TxCode:");
      if (irrecv.decode(&results)) PrintDecodedResult(&results);
      else {
        Serial.println("Unknown encoding.");
        if (PLShow > 0) {
          PrintLeadingSpace(IndexEnd - IndexStart +1, 3);
          Serial.print(IndexEnd - IndexStart +1, DEC);
          Serial.print(" Pulses:");
          PrintPulsesLength(0, IndexEnd - IndexStart +1, USECPERTICK);
          Serial.println("");
        }        
      }
      irparams.rawbuf[0] = irparams.rawbuf[IndexEnd - IndexStart +1];
      IndexStart = IndexEnd + 1;
    }
    Serial.println("");
  }
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

void TxCode() {         // Transmit IR using library irsend.sendXXXX 
  /*
  Currently support NEC, Sharp(in raw format and Address+Command format),
  Panasonic and Sony protocol.
  */
  
  NumParaLen = 3;       // Usually 3 numeric parameters are required,
                        // 2 used by irsend.sendXXXX from ir_XXXX.cpp,
                        // plus 1 for repeat.
  NumPara[0] = 0;
  NumPara[1] = 0;
  NumPara[2] = 0;       // default, no repeat
  
  if (CMD.equalsIgnoreCase("NEC")) {
    /*
    NEC Data Nbits Repeat
      Data      unsigned long
      Nbits     int, optional, max = 32, default = 32 if omitted
      Repeat    int, optional, default = 0 if omitted or Nbits is
                omitted
    */
    CMD = "NEC";
    // Fixed stream length, hence Gap will be calculated later
    NumPara[1] = 32;    // default Nbits for NEC
  }
  else if (CMD.equalsIgnoreCase("Sharp")) {
    /*
    Sharp Address Command Repeat
      Address    unsigned int
      Command    int
      Repeat     int, optional, default = 0 if omitted
    */
    CMD = "Sharp";
    Gap = 0;       // fixed gap of 40ms is already included in sendSharpRaw,
                   // hence no extra gap is required.
  }
  else if (CMD.equalsIgnoreCase("SharpRaw")) {
    /*
    SharpRaw Data Nbits Repeat
      Data      unsigned long
      Nbits     int, optional, default = 15 if omitted
      Repeat    int, optional, default = 0 if omitted or Nbits is omitted
    */
    CMD = "SharpRaw";
    Gap = 0;       // fixed gap of 40ms is already included in sendSharpRaw,
                   // hence no extra gap is required.
    NumPara[1] = 15;    // default Nbits for SharpRaw
  }
  else if (CMD.equalsIgnoreCase("Panasonic")) {
    /*
    Panasonic Address Data Repeat
      Address    unsigned int
      Data       unsigned long
      Repeat     int, optional, default = 0 if omitted
    */
    CMD = "Panasonic";
    Gap = 75000;        // fixed gap length of 75ms
  }
  else if (CMD.equalsIgnoreCase("Sony")) {
    /*
    Sony Data Nbits Repeat
      Data      unsigned long
      Nbits     int
      Repeat    int, optional, default = 0 if omitted
  */
    CMD = "Sony";
    // Fixed stream length, hence Gap will be calculated later
  }
  /*  for other send command
  else if (CMD.equalsIgnoreCase("XXXX")) {
    CMD = "XXXX";       // set command name
    Gap = X;            // set Gap length here if it is a fixed value
    NumParaLen = X;     // set no. of numeric parameters
    NumPara[x] = X;     // set default value, if any
  }
  */
  
  if (TxShow) Serial.print(CMD);
  
  // read numeric parameters
  myError = false;
  int i = 0;
  while ((!myError) && (i < NumParaLen) && (ThisChar != 10)) {
    NumPara[i] = ReadOneValue();
    if (TxShow) {
      PrintNumeric(NumPara[i], bdh);
    }
    i++;
  }
  if (myError) {
    PrintError();
    if (ThisChar != 10) SerialInClear();
    return;
  }
  
  if (TxShow) {
    while (i < NumParaLen) {
      Serial.print(" ");
      Serial.print(NumPara[i], DEC);
      i++;
    }
    Serial.println("");
  }
  
  Repeat = NumPara[NumParaLen -1];     // Repeat = final parameters
  
  disableIRIn();
  BLINKLED_ON();
  
  if (CMD == "NEC") {
    // some values defined in ir_NEC.cpp
    // they are required for repeat stream and calculates gap length
    unsigned int myNEC_HDR_MARK = 9000;
    unsigned int myNEC_HDR_SPACE = 4500;
    unsigned int myNEC_BIT_MARK = 560;
    unsigned int myNEC_ONE_SPACE = 1690;
    unsigned int myNEC_ZERO_SPACE = 560;
    unsigned int myNEC_RPT_SPACE = 2250;

    // fixed length of 110ms from start to start
    Gap = 110000ul - myNEC_HDR_MARK - myNEC_HDR_SPACE;
    for (i = 0; i < NumPara[1]; i++) {
      if (bitRead(NumPara[0], i) == 1) {
        Gap = Gap - myNEC_BIT_MARK - myNEC_ONE_SPACE;
      }
      else {
        Gap = Gap - myNEC_BIT_MARK - myNEC_ZERO_SPACE;
      }
    }
    
    // transmits NEC main stream
    irsend.sendNEC(NumPara[0], NumPara[1]);
    // first gap
    delay(Gap/1000);
    delayMicroseconds(Gap % 1000);
    
    // transmits NEC repeat stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.mark(myNEC_HDR_MARK);
      irsend.space(myNEC_RPT_SPACE);
      irsend.mark(myNEC_BIT_MARK);
      irsend.space(0);
      // fixed gap of 98,190us for repeats
      delay(98);
      delayMicroseconds(190);
      
      if ((Repeat < 1000) && (Repeat > 0)) Repeat--;
    }
  }
  else if (CMD == "SharpRaw") {
    if (Repeat < 1001) Repeat++;    // add one for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.sendSharpRaw(NumPara[0], NumPara[1]);
      //delay(Gap / 1000);   // 40ms repeat gap is already included in sendSharpRaw
      if ((Repeat < 1001) && (Repeat > 0)) Repeat--;
    }
  }
  else if (CMD == "Sharp") {
    if (Repeat < 1001) Repeat++;    // add one for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.sendSharp(NumPara[0], NumPara[1]);
      //delay(Gap / 1000);   // 40ms repeat gap is already included in sendSharp
      if ((Repeat < 1001) && (Repeat > 0)) Repeat--;
    }
  }
  else if (CMD == "Panasonic") {
    if (Repeat < 1001) Repeat++;    // add one for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.sendPanasonic(NumPara[0], NumPara[1]);
      delay(Gap / 1000);
      //delayMicroseconds(Gap % 1000);      // not necessary, since Gap is int of ms
      if ((Repeat < 1001) && (Repeat > 0)) Repeat--;
    }
  }
  else if (CMD == "Sony") {
    // some values defined in ir_Sony.cpp
    // they are required for calculating gap length
    unsigned int mySONY_HDR_MARK = 2400;
    unsigned int mySONY_HDR_SPACE = 600;
    unsigned int mySONY_ONE_MARK = 1200;
    unsigned int mySONY_ZERO_MARK = 600;
    
    // fixed length = 45ms from start to start
    Gap = 45000ul - mySONY_HDR_MARK;
    for (i = 0; i < NumPara[1]; i++) {
      if (bitRead(NumPara[0], i) == 1) {
        Gap = Gap - mySONY_ONE_MARK - mySONY_HDR_SPACE;
      }
      else {
        Gap = Gap - mySONY_ZERO_MARK - mySONY_HDR_SPACE;
      }
    }
    
    if (Repeat< 1001) Repeat++;    // add one for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.sendSony(NumPara[0], NumPara[1]);
      delay(Gap/1000);
      delayMicroseconds(Gap % 1000);
      if ((Repeat < 1001) && (Repeat> 0)) Repeat--;
    }
  }
  /*     for other send command
  else if (CMD == "XXXX") {
    // calculates Gap for fixed stream length protocol, such as NEC, Sony
    Gap = ????
    if (Repeat < 1001) Repeat++;    // add one for initial stream
    while ((Repeat > 0) && (!Serial.available())){
      irsend.sendXXXX(NumPara[0], NumPara[1]);
      delay(Gap/1000);
      delayMicroseconds(Gap % 1000);
      if ((Repeat < 1001) && (Repeat > 0)) Repeat--;
    }
  }
  */
  
  if (Repeat != 0) {
    Serial.println("Repeats aborted.");
    if (!TxShow) Serial.println("");
  }
  
  if (TxShow) Serial.println("");
  BLINKLED_OFF();
}

void ReadPulses(){           // Read incoming IR stream pulse length.
  /* 
  ReadPulses Gap
    Gap....min gap length, in us, between each IR streams.
           max = 1000000us(1s).
           default = _GAP if omitted, that is defined in IRremoteInt.h
           max. no. of pulses = RAWBUF -1(or -2 if RAWBUF is even)
           while RAWBUF is defined in IRremoteInt.h, same for Pulses.
  */
  
  disableIRIn();
  
  boolean RxState;
  byte LeadingSpace;
  
  if (ThisChar == 10) Gap = _GAP;      // default, defined in  IRremoteInt.h
  else Gap = ReadOneValue();
  if (Gap > 1000000) Gap = 1000000;
  
  if (ThisChar != 10) SerialInClear();
  
  Serial.print("ReadPulses ");
  Serial.print(Gap, DEC);
  if (myError) PrintError();
  else {                                    // get ready to read
    Serial.println(". Press a key within 5s....");
    Serial.end();
    RawLen = -1;
    RxState = digitalRead(RECV_PIN);
    TimeStart = micros();
    TimeEnd = TimeStart;
    TimeQuit = TimeStart + 5000000;    // quit if no pulse arrive within 5s
    while ((RawLen <= BufMaxIndex - 1) && (TimeQuit >= micros())) {    // start reading
      if (RxState != digitalRead(RECV_PIN)) {
        TimeEnd = micros();
        RawLen++;
        digitalWrite(LED_BUILTIN, RxState);
        RxState = !RxState;
        myULTemp = TimeEnd - TimeStart;
        irparams.rawbuf[RawLen] = BufTime2Raw(myULTemp);
        TimeStart = TimeEnd;
        TimeQuit = TimeStart + 1000000;     // quit if no more pulse arrive within 1s
      }
    }
    digitalWrite(LED_BUILTIN, 0);
    Serial.begin(myBaud);
    if (RawLen > 1) {
      Serial.print(RawLen, DEC);
      Serial.print(" puluses were read.");
    }
    if (RawLen % 2) {
      RawLen++;
      irparams.rawbuf[RawLen] = 65535;
      Serial.print(" An extra space of 1s is added to the end.");
    }
    Serial.println("");
    
    Gap = BufTime2Raw(Gap);                 // get ready to decode
    if (Gap > 65535) Gap = 65535;
    irparams.rawbuf[0] = 65535;
    irparams.rawbuf[BufMaxIndex] = 65535;
    IndexStart = 1;
    while (IndexStart < RawLen) {           // try to decode
      IndexEnd = IndexStart;

      // look for a gap
      while ((IndexEnd % 2) || ((irparams.rawbuf[IndexEnd] < Gap) && (IndexEnd < RawLen))) IndexEnd++;
      
      myULTemp = IndexEnd -IndexStart +1;
      irparams.rawlen = (unsigned int) myULTemp;
      Serial.println("RxPulses:");
      Serial.print("Pulses kHz");
      PrintLeadingSpace(myULTemp, 4);
      Serial.print(irparams.rawlen, DEC);
      Serial.print(" Repeat");
      PrintPulsesLength(IndexStart, IndexEnd, 1);
      for (int i = IndexStart; i<= IndexEnd; i++) {        // change buffer values to tick
        irparams.rawbuf[i - IndexStart +1] = BufRaw2Time(irparams.rawbuf[i]) / USECPERTICK;
      }
      if (irparams.rawlen %2) {
        irparams.rawlen++;
        irparams.rawbuf[irparams.rawlen] = 65535;
      }
      Serial.println("");
      irparams.rcvstate = STATE_STOP;
      Serial.println("RxCode:");
      if (irrecv.decode(&results)) {
        PrintDecodedResult(&results);
      }
      else {
        Serial.println("Unknown encoding.");
        PrintLeadingSpace(IndexEnd - IndexStart +1, 3);
        Serial.print(IndexEnd - IndexStart +1, DEC);
        Serial.print(" Pulses:");
        PrintPulsesLength(0, IndexEnd - IndexStart, USECPERTICK);
        Serial.println("");
      }
      Serial.println("");
      irparams.rawbuf[0] = irparams.rawbuf[IndexEnd -IndexStart +1];
      IndexStart = IndexEnd + 1;
    }
  }
}

String ReadCMD() {      // Serial read until next char is a Tab, NL or Space.
  String Result = "";
  SerialWait();
  while (!isSpace(NextChar)) {
    ThisChar = Serial.read();
    Result += ThisChar;
    SerialWait();  
  }
  SerialSkip();
  return Result;
}

unsigned long ReadOneValue() {    // read one numeric value
  unsigned long Result = 0;
  bdh = 10;                       // Bin/Dec/Hec, default = 10
  SerialWait();
  ThisChar = Serial.read();       // read first char
  SerialWait();
  if ((ThisChar >=65) && (ThisChar <= 90)) ThisChar = ThisChar + 32;      // shift to lower case
  if (ThisChar == 'b') bdh = 2;         // bin
  else if (ThisChar == 'h') bdh = 16;   // hex
  else if ((ThisChar >= '0') && (ThisChar <= '9')) Result = ThisChar - 48;  // dec 0~9
  else if            // error if not m, s, hyphen or comma
  (!((ThisChar == 'm') || (ThisChar == 's') || (ThisChar == '-') || (ThisChar == ','))) {
    myError = true;
  }
  
  while (!isSpace(NextChar)) {    // read until NextChar is a Tab, NL or Space
    SerialWait();
    ThisChar = Serial.read();
    SerialWait();
    if (isUpperCase(ThisChar)) ThisChar = ThisChar + 32;    // shift to lower case
    if ((ThisChar == 'm') || (ThisChar == 's') || (ThisChar == '-') || (ThisChar == ',')) {
      // ignore m, s, minus, comma
    }
    else if (bdh == 2) {
      if (ThisChar == '0') Result = Result * 2;
      else if (ThisChar == '1') Result = Result * 2 +1;
      else myError = true;
    }
    else if (bdh == 10) {
      if ((ThisChar >= '0') && (ThisChar <='9')) Result = Result * 10 + ThisChar - 48;
      else myError = true;
    }
    else if (bdh == 16){
      if ((ThisChar >= '0') && (ThisChar <='9')) Result = Result * 16 + ThisChar - 48;
      else if ((ThisChar >= 'a') && (ThisChar <='f')) Result = Result * 16 + ThisChar - 87;
      else myError = true;
    }
  }
  SerialSkip();
  return Result;
}

unsigned int BufTime2Raw(unsigned long Time) {
  /*
    In order to store pulse length up to 1000000us(1s), in unsigned int,
    simple compression is used. First of all, micros() always return a value
    that is a multiple of 4, it makes sense to store Time/4. But this is
    not enough since the max pulse length will be approx. 260ms(4 * 65535).
    Hence pulse length is stored in two ways, Time/4 is stored for pulse
    length shorter than 114568us, while longer pulse length is stored in step
    of 24.
  */
  if (Time > 1000000) Time = 1000000;
  if (Time <= 114568) return (unsigned int) (Time /4);
  else return (unsigned int) ((Time -114545) / 24 +28642);
}

unsigned long BufRaw2Time(unsigned int Raw) {
  // Reverse of BufTime2Raw
  if (Raw >= 28643) return (24ul * (Raw -28642)  + 114568);
  else return (4ul * Raw);
}

void disableIRIn() {
  TIMER_DISABLE_INTR;
}

void PrintDecodedResult(decode_results *results) {    // Print decoded result
  int count = results->rawlen;
  
  if (results->decode_type == NEC) {             // NEC Data Nbits Repeat
    Serial.print("NEC h");
    Serial.print(results->value, HEX);
    Serial.print(" ");
    Serial.println(results->bits, DEC);
  } 
  else if (results->decode_type == SHARP) {      // Sharp
    Serial.print("Sharp h");                     // Sharp Address Command Repeat
    NumPara[0] = results->value;                 // use NumPara[0] as temp
    NumPara[0] = (NumPara[0] >> 10) & 31;        // Address
    Serial.print(NumPara[0], HEX);
    Serial.print(" h");
    NumPara[0] = results->value;                 // Command
    NumPara[0] = (NumPara[0] >> 2) & 255;
    if ((results->value & 3) == 2) Serial.print(NumPara[0], HEX);
    else Serial.print(NumPara[0] ^ 255, HEX);
    
    if ((results->value & 3) == 2)      Serial.print(", Normal,  ");
    else if ((results->value & 3) == 1) Serial.print(", Inverted,");
    else                                Serial.print(", Unknown, ");

    Serial.print(" SharpRaw h");                // SharpRaw Data Nbits Repeat
    Serial.print(results->value, HEX);
    Serial.print(" ");
    Serial.println(results->bits, DEC);
  }
  else if (results->decode_type == SONY) {       // Sony Data Nbits Repeat
    Serial.print("Sony h");
    Serial.print(results->value, HEX);
    Serial.print(" ");
    Serial.println(results->bits, DEC);
  } 
  else if (results->decode_type == PANASONIC) {  // Panasonic Address Data Repeat
    Serial.print("Panasonic h");
    Serial.print(results->address, HEX);
    Serial.print(" h");
    Serial.println(results->value, HEX);
  }
  else {                                         // Others
    if (results->decode_type == UNKNOWN)           Serial.print("Unknown encoding: ");
    else if (results->decode_type == RC5)          Serial.print("RC5: ");
    else if (results->decode_type == RC6)          Serial.print("RC6: ");
    else if (results->decode_type == LG)           Serial.print("LG: ");
    else if (results->decode_type == JVC)          Serial.print("JVC: ");
    else if (results->decode_type == WHYNTER)      Serial.print("Whynter: ");
    else if (results->decode_type == DENON)        Serial.print("Denon: "); 
    else if (results->decode_type == SANYO)        Serial.print("SANYO: "); 
    else if (results->decode_type == DISH)         Serial.print("DISH: "); 
    else if (results->decode_type == MITSUBISHI)   Serial.print("MITSUBISHI: "); 
    else if (results->decode_type == SAMSUNG)      Serial.print("SAMSUNG: "); 
    else if (results->decode_type == AIWA_RC_T501) Serial.print("AIWA_RC_T501: "); 
    Serial.print(results->value, HEX);
    Serial.print(" (");
    Serial.print(results->bits, DEC);
    Serial.println(" bits)");
  }
  if (PLShow > 0) {
    if (PLShow == 1) {
      Serial.print("       Gap:");
      PrintLeadingSpace((unsigned long) irparams.rawbuf[0] * USECPERTICK, 8);
      Serial.print("s");
      Serial.println((unsigned long) irparams.rawbuf[0] * USECPERTICK);
    }
    else if (((PLShow == 2) && (results->decode_type == UNKNOWN)) || (PLShow == 3)) {
      PrintLeadingSpace(irparams.rawlen, 3);
      Serial.print(irparams.rawlen, DEC);
      Serial.print(" Pulses:");
      PrintPulsesLength(0, irparams.rawlen - 1, USECPERTICK);
      Serial.println("");
    }
  }
}

void PrintError() {
  Serial.println("Error");
  Serial.println("");
}

void PrintLeadingSpace(unsigned long Time, byte Width) {
  if (Time == 0) Time++;
  while (Time) {
    Time = Time / 10;
    Width--;
  }
  while (Width) {
    Serial.print(" ");
    Width--;
  }
}

void PrintNumeric(unsigned long pNum, byte pBDH) {
  Serial.print(" ");
  if      (pBDH == 2) {
    Serial.print("b");
    Serial.print(pNum, BIN);
  }
  else if (pBDH == 16) {
    Serial.print("h");
    Serial.print(pNum, HEX);
  }
  else Serial.print(pNum, DEC);
}

void PrintPulsesLength(int IndexStart, int IndexEnd, int Scale) {
  for (int i = IndexStart; i <= IndexEnd; i++) {
    if (Scale == USECPERTICK) myULTemp = (unsigned long) irparams.rawbuf[i] * Scale;
    else myULTemp = BufRaw2Time(irparams.rawbuf[i]) * Scale;
    PrintLeadingSpace(myULTemp, 8);
    if (i % 2) Serial.print("m");
    else Serial.print("s");
    Serial.print(myULTemp, DEC);
  }
}

void PrintStatus() {
  Serial.print("Status: Rx");
  if (RxEnable) Serial.print("Enable");
  else Serial.print("Disable");
  
  Serial.print(", Tx");
  if (TxShow) Serial.print("Show");
  else Serial.print("Hide");
  
  Serial.print(", PL");
  switch (PLShow) {
    case 0:
      Serial.println("Hide");
      break;
    case 1:
      Serial.println("Gap");
      break;
    case 2:
      Serial.println("Unknown");
      break;
    case 3:
      Serial.println("Show");
  }
  Serial.println("");
}

void SerialWait() {               // wait for next serial in
  while (!Serial.available()) {
    // do nothing
  }
  NextChar = Serial.peek();
}

void SerialInClear() {            // discard SerialIn until NL
  SerialWait();
  ThisChar = Serial.read();
  while ((ThisChar != 10)) {// && (Serial.available())) {
    SerialWait();
    ThisChar = Serial.read();
  }
  NextChar = Serial.peek();
}

void SerialSkip() {               // skip space, Tab, NL
  SerialWait();
  while ((isSpace(NextChar)) && (NextChar != 10))  {
    ThisChar = Serial.read();
    SerialWait();
  }
  if (NextChar == 10) {
    ThisChar = Serial.read();
    NextChar = Serial.peek();
  }
}
