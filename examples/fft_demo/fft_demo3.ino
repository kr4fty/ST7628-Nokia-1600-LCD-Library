/***************************************************
  This is a library for the ST7628 SPI display.

  These displays use SPI 9-bits to communicate, 3 or 4 pins are required to
  interface (RST is optional)
  
  This example shows an implementation of the Fast Fourier Transform (FFT), 
  sampled by the analog PIN 0.
  
  Pines used:
    * Audio:
               - Audio in:  A0
    * Display:
               - CS:        5
               - RST        4
               - SCLK       27
               - SDAT       26
  
  
  Author: Tapia Favio <technicc@gmail.com>

  It depends on the following libraries:
   - ST7628: https://github.com/kr4fty/ST7628
   - arduinoFFT: https://github.com/kosme/arduinoFFT
   - Adafruit-GFX: https://github.com/adafruit/Adafruit-GFX-Library
  
****************************************************/

#include <Adafruit_GFX.h>
#include <ST7628.h>
#include <arduinoFFT.h> // include the library

arduinoFFT FFT = arduinoFFT(); // Create FFT object
/* 
These values can be changed in order to evaluate the functions 
*/
const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2

double vReal[samples];
double vImag[samples];

uint8_t amp_previa[samples];
uint8_t samplingCont=0;
uint8_t peak[samples];
uint8_t diffPeak;
#define fallPeakSpeed 2

#define TFT_CS     5
#define TFT_RST    4
#define TFT_SCLK   27
#define TFT_MOSI   26
ST7628 tft = ST7628(TFT_CS, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  tft.init();
  tft.fillScreen(ST7628_WHITE);
  
  // Configure the port interrupt 0, analog, to take audio samples for later analysis by FFT.
  cli();        //disable interrupts  
  //set up continuous sampling of analog pin 0
  ADCSRA = 0;  //clear ADCSRA and ADCSRB registers
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0);  //set reference voltage
  ADMUX |= (1 << ADLAR);  //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE);  //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  //enable ADC
  ADCSRA |= (1 << ADSC);  //start ADC measurements
  
  sei();//enable interrupts
}

void loop() {
  while(1) { 
    
    while(ADCSRA & _BV(ADIE));  // Wait for the buffer to fill
    
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    
    drawSpectrum(vReal, samples/2);
    
    samplingCont=0;
    ADCSRA |= _BV(ADIE);      // Interrupt on
  }
}

void drawSpectrum(double *fft_buffer, int n)
{
    int i;
    uint8_t amp_actual;
    int x_max= tft.width();
    int y_max= tft.height();
    
    for(i=2;i<n;i++){
        // amplitud del espectro
        amp_actual= (uint8_t)fft_buffer[i]/5;
        
        if(amp_previa[i] > amp_actual)
          tft.drawFastVLine(i, y_max-amp_previa[i], amp_previa[i]-amp_actual, ST7628_WHITE);
        
        if(amp_actual > amp_previa[i])
          tft.drawFastVLine(i, y_max-amp_actual, amp_actual-amp_previa[i], ST7628_GREEN);
        amp_previa[i] = amp_actual;
        
        // Effect drop slow peaks 
        if(peak[i]>amp_actual){
          diffPeak= peak[i]-amp_actual;
          tft.drawPixel(i, y_max-peak[i], ST7628_WHITE);
          if( diffPeak>40 )
            peak[i]-=fallPeakSpeed*16;
          else if( diffPeak>30 )
            peak[i]-=fallPeakSpeed*8;
          else if( diffPeak>20 )
            peak[i]-=fallPeakSpeed*4;
          else if( diffPeak>10 )
            peak[i]-=fallPeakSpeed*2;
          else
            peak[i]-=fallPeakSpeed;
        }
        else{
          tft.drawPixel(i, y_max-peak[i], ST7628_GREEN);
          peak[i]=amp_actual;
        }
        tft.drawPixel(i, y_max-peak[i], ST7628_RED);
    }
     
}

ISR(ADC_vect) {//when new ADC value ready
  if(samplingCont<samples){
    vReal[samplingCont] = (uint8_t)ADCH;
    vImag[samplingCont] = 0.0;
    samplingCont++;
  }
  else
    ADCSRA &= ~_BV(ADIE);  // Buffer full, interrupt off
}
