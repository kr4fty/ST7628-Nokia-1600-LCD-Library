# ST7628
ST7628 lcd library for Arduino


Display Nokia 2310, chipset ST7628, compatible with:

     Nokia 1600, 1208, 1209, 2310, 2126 CDMA, (6125small, 6136, N71small)

## Pinouts
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

## Videos - FFT demo

  * https://www.youtube.com/watch?v=IAP3RYwZHVA
  
  * https://www.youtube.com/watch?v=7KjgSyl-0yg

### Author

Tapia Favio <technicc@gmail.com>
