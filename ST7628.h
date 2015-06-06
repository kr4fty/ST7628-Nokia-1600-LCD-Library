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

#ifndef _ST7628H_
#define _ST7628H_

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif


#define ST7628_TFTWIDTH  97
#define ST7628_TFTHEIGHT 69

#define ST7628_NOP     0x00
#define ST7628_SWRESET 0x01
#define ST7628_RDDID   0x04
#define ST7628_RDDST   0x09

#define ST7628_SLPIN   0x10
#define ST7628_SLPOUT  0x11
#define ST7628_PTLON   0x12
#define ST7628_NORON   0x13

#define ST7628_INVOFF  0x20
#define ST7628_INVON   0x21
#define ST7628_WRCNTR 0x25
#define ST7628_DISPOFF 0x28
#define ST7628_DISPON  0x29
#define ST7628_CASET   0x2A
#define ST7628_RASET   0x2B
#define ST7628_RAMWR   0x2C
#define ST7628_RAMRD   0x2E

#define ST7628_PTLAR   0x30
#define ST7628_COLMOD  0x3A
#define ST7628_MADCTL  0x36
#define ST7628_IDMOFF 0x38

#define ST7628_FRMCTR1 0xB1
#define ST7628_FRMCTR2 0xB2
#define ST7628_FRMCTR3 0xB3
#define ST7628_INVCTR  0xB4
#define ST7628_NLIS       0xB5
#define ST7628_DISSET5 0xB6

#define ST7628_PWCTR1  0xC0
#define ST7628_PWCTR2  0xC1
#define ST7628_PWCTR3  0xC2
#define ST7628_PWCTR4  0xC3
#define ST7628_PWCTR5  0xC4
#define ST7628_VMCTR1  0xC5
#define ST7628_VSS         0xCB
#define ST7628_ID1S        0xCC
#define ST7628_ID3S        0xCE

#define ST7628_ANASET 0xD0
#define ST7628_ALS       0xD7
#define ST7628_RDID1   0xDA
#define ST7628_RDID2   0xDB
#define ST7628_RDID3   0xDC
#define ST7628_RDID4   0xDD

#define ST7628_PWCTR6  0xFC
#define ST7628_THYS       0xF7

#define ST7628_GMCTRP1 0xE0
#define ST7628_GMCTRN1 0xE1
#define ST7628_EPMRD     0xE3

// Color definitions
#define	ST7628_BLACK   0x0000
#define	ST7628_BLUE    0x001F
#define	ST7628_RED     0xF800
#define	ST7628_GREEN   0x07E0
#define ST7628_CYAN    0x07FF
#define ST7628_MAGENTA 0xF81F
#define ST7628_YELLOW  0xFFE0  
#define ST7628_WHITE   0xFFFF


class ST7628 : public Adafruit_GFX {

 public:

  ST7628(int8_t CS, int8_t SID, int8_t SCLK, int8_t RST);

  void     init(void),                             // for ST7628 displays
           setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
             uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i);
  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  /* These are not for current use, 8-bit protocol only!
  uint8_t  readdata(void),
           readcommand8(uint8_t);
  uint16_t readcommand16(uint8_t);
  uint32_t readcommand32(uint8_t);
  void     dummyclock(void);
  */

 private:
  uint8_t  tabcolor;

  void     spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           commandList(const uint8_t *addr),
           commonInit(const uint8_t *cmdList);
//uint8_t  spiread(void);

#if defined(__AVR__) || defined(CORE_TEENSY)
volatile uint8_t *dataport, *clkport, *csport;
  uint8_t  _cs, _rst, _sid, _sclk,
           datapinmask, clkpinmask, cspinmask, rspinmask,
           colstart, rowstart; // some displays need this changed
#endif //  #ifdef __AVR__
  
};

#endif
