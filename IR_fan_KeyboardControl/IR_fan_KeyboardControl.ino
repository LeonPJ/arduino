//紅外線發射器須接到D3(nano)
#include <IRremote.h>                    // 引用 IRRemote 函式庫
IRsend irsend;                           // 定義 IRsend 物件來發射紅外線訊號

void setup()
{
  Serial.begin(9600);                   // 啟用串列通訊並設定波特率
}

int i;

void loop() 
{
  if (Serial.available() > 0)// 檢查是否有資料可供讀取
    {
    char order = Serial.read();
    switch(order)
      {
      case 43://加風速 +
        Serial.println("這是加");
        irsend.sendNEC(0x807F8F70, 32);// decode
        break;

      case 45://減風速 -
        Serial.println("這是減");
        irsend.sendNEC(0x807F0FF0, 32);
        break;

      case 115://小寫s ASCII 碼 115
        Serial.println("開/關");
        irsend.sendNEC(0x807F7986, 32);
        break;

      case 109://小寫m ASCII 碼 109
        Serial.println("擺頭");
        irsend.sendNEC(0x807FBB44, 32);
        break;

      case 79://大寫O定時開 ASCII 碼 79
        Serial.println("定時開");
        irsend.sendNEC(0x807FD32C, 32);
        break;

      case 111://小寫o定時關 ASCII 碼 111
        Serial.println("定時關");
        irsend.sendNEC(0x807F3BC4, 32);
        break;

      case 10://new line ASCII
        break;

      case 13://carriage return ASCII
        break;
        
      default:
        Serial.println("錯誤輸入");
        break;
      }
  }
}
