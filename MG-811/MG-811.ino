/*******************Demo for MG-811 Gas Sensor Module V1.1*****************************
Author:  Tiequan Shao: tiequan.shao@sandboxelectronics.com
         Peng Wei:     peng.wei@sandboxelectronics.com
          
Lisence: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
 
Note:    This piece of source code is supposed to be used as a demostration ONLY. More
         sophisticated calibration is required for industrial field application. 
          
                                                    Sandbox Electronics    2012-05-31

Modify by ChihhsiangChien
************************************************************************************/
 
/************************Hardware Related Macros************************************/
#define         MG_PIN                       (0)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (2)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier
 
 
/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in 
                                                     //normal operation
 
/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         V400        (984)                 //測量室外空氣得到的AnalogValue(400ppm)
#define         V40000   (745)                  //測量呼氣得到的AnalogValue(40000ppm)
 
/*****************************Globals***********************************************/
 float slope=(V40000-V400)/(4.602-2.602);
           //校正線的斜率計算
           //log40000=4.602
           //log400=2.602

int DisplayMode=2; //1是呈現資訊  2是只有二氧化碳濃度，可供Serial Plot
void setup()
{
    Serial.begin(115200);                              //UART setup, baudrate = 115200bps
    pinMode(BOOL_PIN, INPUT);                        //set pin to input
    digitalWrite(BOOL_PIN, HIGH);                    //turn on pullup resistors


}
 
void loop()
{
    int percentage;
    float analogValue;
    float volts;
     
    
    analogValue = MGRead(MG_PIN);
    volts=analogValue*5/1024;
    percentage = MGGetPercentage(analogValue);

if (DisplayMode==1){
    Serial.print(analogValue);
    Serial.print( "\t" );
    
    Serial.print(volts); 
    Serial.print( "V" );
    Serial.print( "\t" );
    
    Serial.print("CO2:");
    Serial.print(percentage);
    Serial.print( "ppm" );  
    Serial.print( "\t" );
    
    Serial.print( "Time point:" );
    Serial.print(millis());
    Serial.print("\n"); 
  }else{
    Serial.println(percentage);
  }
     
    delay(200);
}
 
 
 
/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of SEN-000007
Remarks: This function reads the output of SEN-000007
************************************************************************************/
float MGRead(int mg_pin)
{
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v =v/READ_SAMPLE_TIMES ;
    return v;  
}
 
/*****************************  MQGetPercentage **********************************
輸入讀入的AnalogValue，利用公式轉換成二氧化碳濃度
二氧化碳濃度的對數值與AnalogValue相關，
************************************************************************************/
int  MGGetPercentage(float analogValue)
{   
     float logConc=2.602 +(analogValue-V400)/slope; //計算目前二氧化碳的濃度log值
      return pow(10,logConc );
 
}
