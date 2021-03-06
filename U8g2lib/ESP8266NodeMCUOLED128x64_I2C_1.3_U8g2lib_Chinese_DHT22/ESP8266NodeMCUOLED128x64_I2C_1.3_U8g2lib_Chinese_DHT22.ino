#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE,D1,D2);

void u8g2_prepare(void) {
  //u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFont(u8g2_font_ncenR24_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

/*void u8g2_box_title(uint8_t a) {
  u8g2.drawStr( 10+a*2, 5, "U8g2");
  u8g2.drawStr( 10, 20, "GraphicsTest");
  
  u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight() );
}*/

#define cross_width 32
#define cross_height 32
static const unsigned char cross_bits[] U8X8_PROGMEM  = {// 32x32 溫
0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,
0x00,0x00,0x7F,0x7E,
0x00,0x00,0x7F,0x7E,
0x00,0x40,0x61,0x0C,
0x00,0x60,0x61,0x0C,
0x00,0x30,0x61,0x0C,
0x00,0x18,0x61,0x0C,
0x00,0x0C,0x61,0x0C,
0x00,0x04,0x0C,0x8C,
0x00,0x00,0x64,0x4C,
0x00,0x40,0x68,0x16,
0x00,0x60,0x70,0x18,
0x00,0x30,0x7F,0x7E,
0x00,0x18,0x7F,0x7E,
0x00,0x0C,0x00,0x00,

0x00,0x04,0x7F,0x7E,
0x00,0x00,0x7F,0x7E,
0x00,0x00,0x66,0xCC,
0x00,0x00,0x66,0xCC,
0x00,0x00,0x66,0xCC,
0x00,0x04,0x66,0xCC,
0x00,0x0C,0x66,0xCC,
0x00,0x18,0x66,0xCC,
0x00,0x30,0x66,0xCC,
0x00,0x60,0x66,0xCC,
0x00,0xC0,0x66,0xCC,
0x01,0x80,0x66,0xCC,
0x01,0x00,0x66,0xCC,
0x00,0x00,0x00,0xCC,
0x00,0x00,0x00,0xCC,
0x00,0x00,0x00,0x00,
};

void u8g2_bitmap_modes(uint8_t transparent) {
  /*const uint8_t frame_size = 24;

  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  u8g2.drawStr(frame_size * 2, 50, "White");
  u8g2.drawStr(frame_size * 3.5, 50, "XOR");*/

  //u8g2.setDrawColor(0);// Black
  //u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(1); // White
  u8g2.drawXBMP(0, 32, cross_width, cross_height, cross_bits);
  //u8g2.drawXBMP(92, 24, cross_width, cross_height, cross_bits2);
  //u8g2.setDrawColor(2); // XOR
  //u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}


void draw(void) {
  u8g2_prepare();
  u8g2_bitmap_modes(0); 
  u8g2_bitmap_modes(1); 
}


void setup(void) {
  u8g2.begin();
}

void loop(void) {
  
  // picture loop  
  u8g2.firstPage();  
  do {
    //draw();
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  //u8g2.drawXBMP(32, 0, cross_width, cross_height, cross_bits);
  u8g2.drawStr(0,32 , "Hello!!");
  } while( u8g2.nextPage() );
  
  delay(1000);

}
