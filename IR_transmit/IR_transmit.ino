//紅外線發射器須接到D3(nano)
#include <IRremote.h>                    // 引用 IRRemote 函式庫
IRsend irsend;                           // 定義 IRsend 物件來發射紅外線訊號

//unsigned int  rawData[67] = {8950,4450, 650,1600, 550,500, 650,500, 650,450, 650,450, 650,500, 650,450, 650,500, 650,450, 650,1550, 650,1600, 600,1600, 650,1600, 550,1650, 600,1600, 600,1650, 550,550, 650,1550, 650,1600, 550,1650, 650,1550, 700,450, 600,500, 700,1550, 600,1600, 650,500, 650,450, 650,450, 650,500, 600,1600, 600,1600, 650,500, 650};  // NEC 807F7986
//unsigned int  rawData[67] = {9050,4350, 600,1600, 650,450, 600,550, 650,450, 600,550, 550,550, 600,500, 650,500, 650,450, 600,1600, 650,1600, 600,1600, 650,1600, 550,1650, 650,1550, 650,1600, 550,550, 650,1550, 600,1650, 650,1550, 600,1650, 600,500, 650,450, 600,1650, 600,1600, 650,500, 550,550, 650,450, 650,500, 650,1550, 650,1550, 650,500, 600};  // NEC 807F7986
//unsigned int  rawData[] = {9000,4400, 700,1500, 700,400, 650,500, 700,400, 650,500, 650,450, 700,400, 600,550, 550,550, 650,1550, 600,1650, 650,1550, 650,1550, 650,1600, 550,1650, 650,1550, 650,500, 550,1650, 600,1600, 600,1600, 650,1600, 650,450, 600,500, 650,1600, 650,1550, 650,450, 650,500, 650,450, 650,500, 650,1550, 650,1550, 600,500, 600};  // NEC 807F7986
//unsigned int  rawData[23] = {1150,500, 1150,550, 300,1350, 1150,500, 1200,500, 300,1350, 350,1350, 300,1350, 350,1350, 300,1350, 350,1300, 1200};  // UNKNOWN A32AB931
//unsigned int  rawData[23] = {1300,350, 1300,400, 500,1150, 1300,350, 1300,400, 450,1250, 450,1200, 450,1200, 450,1250, 450,1200, 450,1200, 1350};  // UNKNOWN A32AB931



void setup()
{
  Serial.begin(9600);                   // 啟用串列通訊並設定波特率
}
//Single 接 D3
void loop() 
{
  // 檢查 Serial Port 是否有資料
  // 有的話，Serial.read() 會回傳非 -1 的數值
  if (Serial.read() != -1)
  {    
    // 發射紅外線訊號
    irsend.sendNEC(0xFEAC02E5,32);   // 紅外線編碼
    //irsend.sendRaw(rawData, 67, 38);
  }
}
