#define IPin A0// I analog pin39
#define VPin A1// V analog pin36
#define SCL A5// OLED SCL pin
#define SDA A4// OLED SDA pin
#define OLEDAddress 0x3C

#define DataTime 16667// Calculat Data Time


int IreadValue, VreadValue, ImaxValue, VmaxValue, IminValue, VminValue, DataCount;
float Iresult, Vresult, Irms, Vrms, ITotalData, VTotalData, IAvgData, VAvgData;
uint32_t start_time, FirstPage;
String OLEDI, OLEDV;



float RMS() {
  ImaxValue = 0;// 電流ADC讀取最大值初始設定為0
  VmaxValue = 0;// 電壓ADC讀取最大值初始設定為0
  IminValue = 1023;// 電流ADC讀取最大值初始設定為4096
  VminValue = 1023;// 電壓ADC讀取最大值初始設定為4096
  
  ITotalData = 0;// 電流n次加總初始設定為0
  VTotalData = 0;// 電壓n次加總初始設定為0
  IAvgData = 0;// 電流平均初始設定為0
  VAvgData = 0;// 電壓平均初始設定為0

  //-for(DataCount = 0; DataCount < 5; DataCount++){
    start_time = micros();
    while((micros()-start_time) <= 1000) {// 取樣時間16.6667ms

      //delayMicroseconds(144);// 取樣次數100次 144
      IreadValue = analogRead(IPin);
      VreadValue = analogRead(VPin);
      //Serial.println(IreadValue);
     if (IreadValue > ImaxValue) {// 電流判斷大小值
        ImaxValue = IreadValue;
      }else if(IreadValue < IminValue) {
        IminValue = IreadValue;
      }
      
      if (VreadValue > VmaxValue) {// 電壓判斷大小值
        VmaxValue = VreadValue;
      }else if(VreadValue < VminValue) {
        VminValue = VreadValue;
      }
      //Status();
    }

    
    Iresult = ((ImaxValue - IminValue) * 5)/1023;
    Vresult = ((VmaxValue - VminValue) * 5)/1023;
    //AmpsRMS = (VRMS * 1000)/mVperAmp;
    //Serial.println(Iresult);
    Irms = ((Iresult/2) * 0.707 * 1000 / 66);// 20A for 100; 30A for 66 
    Vrms = ((Vresult/2) * 0.707 * 257.13);
    //Serial.println(Irms);
    //ITotalData += Irms;
    //VTotalData += Vrms;
    //count++;
  //-}
  
  //IAvgData = (ITotalData/5);
  //VAvgData = (VTotalData/5);
  //Serial.println(count);
  //OLEDI = String("Irms: ") + Irms + String(" A"); 
  //OLEDV = String("Vrms: ") + Vrms + String(" V");
  return Irms;
  //OLEDI = String("Irms: ") + Irms + String(" A"); 
  //OLEDV = String("Vrms: ") + Vrms + String(" V");
 //Serial.println(OLEDI);
 //Serial.println(OLEDV);
}

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);// 電流訊號
  pinMode(A1, INPUT_PULLUP);// 電壓訊號
}

void loop() {
  Serial.println(RMS());
  Serial.println("-------");
}
