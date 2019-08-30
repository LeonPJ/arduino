#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE,D1,D2);


void setup(void) {
  u8g2.begin();  
}

void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenR10_tr);
    u8g2.drawStr(0,24,"Hello World1!");
  } while ( u8g2.nextPage() );
  delay(5000);
    u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenR10_tr);
    u8g2.drawStr(0,24,"Hello World2!");
  } while ( u8g2.nextPage() );
  delay(5000);
}

