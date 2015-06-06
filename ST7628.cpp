/***************************************************
  This is a library for the ST7628 SPI display.

  These displays use SPI 9-bits to communicate, 3 or 4 pins are required to
  interface (RST is optional)
  
  Based on the driver written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  It depends on libraries Adafruit-GFX

  Author: Tapia Favio: technicc(at)gmail.com
 
 Pinout:
        1: SCLK
        2: SDA                                        1  _+--+_ 10
        3: GND                                        2  _+--+_  9
        4: /CS                                        3  _+--+_  8
        5: /RES                                       4  _+--+_  7
        6: +Vled                                      5  _+--+_  6
        7: -Vled                        +-----------------+--+---+
        8: ------                       |+----------------------+|
        9: Vdda +2.8v (2.4v - 3.3V)     ||      Nokia 1600      ||
       10: Vddio +1.8v (1.65v - 3.0v)   ||         LCD          ||
                                        ||       (98x70)        ||
                                        ||    visible 96x68     ||
                                        ||                      ||    
                                        |+----------------------+|
                                        +------------------------+

 ****************************************************/

#include <Adafruit_GFX.h>
#include "ST7628.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"

inline uint16_t swapcolor(uint16_t x) { 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}


// Constructor when using software SPI.  All output pins are configurable.
ST7628::ST7628(int8_t cs, int8_t sid, int8_t sclk, int8_t rst) 
  : Adafruit_GFX(ST7628_TFTWIDTH, ST7628_TFTHEIGHT)
{
  _cs   = cs;
  _sid  = sid;
  _sclk = sclk;
  _rst  = rst;
}

inline void ST7628::spiwrite(uint8_t c) {

  //Serial.println(c, HEX);
  // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) *dataport |=  datapinmask;
      else        *dataport &= ~datapinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;
    }
}


void ST7628::writecommand(uint8_t c) {
  *csport &= ~cspinmask;
  *dataport  &= ~datapinmask;  
  *clkport |=  clkpinmask;
  *clkport &= ~clkpinmask;
  
  //Serial.print("C ");
  spiwrite(c);

  *csport |= cspinmask;
}


void ST7628::writedata(uint8_t c) {
  *csport &= ~cspinmask;
  *dataport |=  datapinmask;  
  *clkport |=  clkpinmask;
  *clkport &= ~clkpinmask;
    
  //Serial.print("D ");
  spiwrite(c);

  *csport |= cspinmask;
} 
 //


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static const uint8_t PROGMEM
  cmd[] = {
    8,
    ST7628_WRCNTR, 1,
      0x39,
    ST7628_SLPOUT, DELAY,
      10,
    ST7628_NORON,  DELAY,
      10,
    ST7628_INVOFF, DELAY,
      10,
    ST7628_IDMOFF, DELAY,
      10,
    ST7628_MADCTL , 1,
      0x00,
    ST7628_COLMOD, 1,
      0x05,
    ST7628_DISPON, DELAY,
      255
    /*20,
    ST7628_ALS, 1,
      0xDF,
    ST7628_GMCTRP1, 1+DELAY,
      0x00,
      10,
    ST7628_EPMRD, DELAY,
      10,
    ST7628_GMCTRN1, DELAY,
      10,
    //-----------disable autoread + Manual read once -------------------------------
    ST7628_SLPOUT, DELAY,
      50,
    //---------------------------------- Sleep OUT ---------------------------------------------
    ST7628_PWCTR1, 2,
      0xD2,
      0x00,
    //----------------------------Set Register------------------------------------------
    ST7628_PWCTR4, 1,
      0x02,
    ST7628_PWCTR5, 1,
      0x07,
    ST7628_VMCTR1, 1,
      0x21,
    ST7628_VSS, 1,
      0x01,
    ST7628_ID1S, 1,
      0x00,
    ST7628_ID3S, 1,
      0x00,
    ST7628_ANASET, 1,
      0x1D,
    ST7628_COLMOD, 1,
      0x05,
    ST7628_MADCTL, 1,
      0x00,
    ST7628_THYS , 1,
      0x06,
    ST7628_NLIS, 1,
      0x01,
    ST7628_CASET, 2,
      0x00,
      0x61,
    ST7628_RASET, 2,
      0x00,
      0x45,
    ST7628_DISPON, DELAY,
      100*/
  };                  //     255 = 500 ms delay

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void ST7628::commandList(const uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


// Initialization code common to both 'B' and 'R' type displays
void ST7628::commonInit(const uint8_t *cmdList) {
  colstart  = rowstart = 0; // May be overridden in init func

  pinMode(_cs, OUTPUT);

  csport    = portOutputRegister(digitalPinToPort(_cs));

  cspinmask = digitalPinToBitMask(_cs);

  pinMode(_sclk, OUTPUT);
  pinMode(_sid , OUTPUT);

  clkport     = portOutputRegister(digitalPinToPort(_sclk));
  dataport    = portOutputRegister(digitalPinToPort(_sid));

  clkpinmask  = digitalPinToBitMask(_sclk);
  datapinmask = digitalPinToBitMask(_sid);

  *clkport   &= ~clkpinmask;
  *dataport  &= ~datapinmask;

  if (_rst) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(500);
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    delay(500);
  }

  if(cmdList) commandList(cmdList);
}


// Initialization for ST7628 screens
void ST7628::init(void) {
  commonInit(cmd);
}

void ST7628::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,
 uint8_t y1) {

  writecommand(ST7628_CASET); // Column addr set
  writedata(x0+colstart);     // XSTART 
  writedata(x1+colstart);     // XEND

  writecommand(ST7628_RASET); // Row addr set
  writedata(y0+rowstart);     // YSTART
  writedata(y1+rowstart);     // YEND

  writecommand(ST7628_RAMWR); // write to RAM

}


void ST7628::pushColor(uint16_t color) {
  
  writedata(color >> 8);
  writedata(color);

}

void ST7628::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
  setAddrWindow(x,y,x,y);
  
  uint8_t hi = color >> 8, lo = color;
  
  writedata(hi);
  writedata(lo);
  
}


void ST7628::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  while(h--){
    writedata(hi);
    writedata(lo);
  }

}


void ST7628::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;

  while (w--) {
    writedata(hi);
    writedata(lo);
    //Serial.println(w);
  }

}



void ST7628::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}



// fill a rectangle
void ST7628::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w, y+h);

  uint8_t hi = color >> 8, lo = color;

  
  for(y=h; y>=0; y--) {
    for(x=w; x>=0; x--) {
      writedata(hi);
      writedata(lo);
    }
  }

}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ST7628::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void ST7628::setRotation(uint8_t m) {

  writecommand(ST7628_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
     _width  = ST7628_TFTWIDTH;
     _height = ST7628_TFTHEIGHT;
     break;
   case 1:
     writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
     _width = ST7628_TFTHEIGHT;
     _height = ST7628_TFTWIDTH;
     break;
  case 2:
     writedata(MADCTL_RGB);
     _width  = ST7628_TFTWIDTH;
     _height = ST7628_TFTHEIGHT;
    break;
   case 3:
     writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
     _width = ST7628_TFTHEIGHT;
     _height = ST7628_TFTWIDTH;
     break;
  }
}


void ST7628::invertDisplay(boolean i) {
  writecommand(i ? ST7628_INVON : ST7628_INVOFF);
}

