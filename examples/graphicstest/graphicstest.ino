/***************************************************
  This is a library for the ST7628 SPI display.

  These displays use SPI 9-bits to communicate, 3 or 4 pins are required to
  interface (RST is optional)
  
  Based on the driver written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <ST7628.h> // Hardware-specific library


// For the breakout, you can use any 2 or 3 pins
#define TFT_CS     5
#define TFT_RST    4 // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_SCLK   27 // set these to be whatever pins you like!
#define TFT_MOSI   26 // set these to be whatever pins you like!
ST7628 tft = ST7628(TFT_CS, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7628 TFT Test");

  tft.init();   // initialize a ST7628 chip

  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(ST7628_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  Serial.println("fillScreen");
  tft.fillScreen(ST7628_BLACK);
  testdrawtext("This is an example of implementation of the ST7628 driver used in the display of the Nokia 1600 phone.", ST7628_WHITE);
  delay(10000);

  // tft print function!
  Serial.println("tftPrintTest");
  tftPrintTest();
  delay(4000);

  // a single pixel
  Serial.println("a single pixel");
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7628_GREEN);
  delay(500);

  // line draw test
  Serial.println("line draw test");
  testlines(ST7628_YELLOW);
  delay(500);

  // optimized lines
  Serial.println("optimized lines");
  testfastlines(ST7628_RED, ST7628_BLUE);
  delay(500);

  Serial.println("testdrawrects");
  testdrawrects(ST7628_GREEN);
  delay(500);

  Serial.println("testfillrects");
  testfillrects(ST7628_YELLOW, ST7628_MAGENTA);
  delay(500);

  tft.fillScreen(ST7628_BLACK);
  Serial.println("testfillcircles");
  testfillcircles(10, ST7628_BLUE);
  Serial.println("testdrawcircles");
  testdrawcircles(10, ST7628_WHITE);
  delay(500);

  Serial.println("testroundrects");
  //testroundrects();
  delay(500);

  Serial.println("testtriangles");
  testtriangles();
  delay(500);

  Serial.println("mediabuttons");
  mediabuttons();
  delay(500);

  Serial.println("done");
  delay(1000);
}

void loop() {
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7628_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7628_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST7628_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST7628_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7628_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7628_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7628_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST7628_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST7628_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7628_BLACK);
  tft.setTextColor(ST7628_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST7628_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST7628_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7628_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7628_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST7628_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7628_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7628_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7628_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7628_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7628_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7628_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7628_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7628_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7628_GREEN);
}