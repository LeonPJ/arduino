//紅外線發射器須接到D3(nano)
#include <IRremote.h>                    // 引用 IRRemote 函式庫
IRsend irsend;                           // 定義 IRsend 物件來發射紅外線訊號

void setup()
{
  Serial.begin(115200);                   // 啟用串列通訊並設定波特率
}
//Single 接 D3
void loop() 
{
  // 檢查 Serial Port 是否有資料
  // 有的話，Serial.read() 會回傳非 -1 的數值
  if (Serial.read() != -1)
  {    
    // 發射紅外線訊號
    irsend.sendNEC(0x807FD32C, 32);   // 紅外線編碼 
  }
}
