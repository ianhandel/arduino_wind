# arduino_wind
Arduino sketch and schematics for anemometer with TFT display and NMEA 0183 output

Uses generic 2.8 TFT

I used Jaycar custom libraries from [here](http://blog.ylett.com/2017/01/playing-with-jaycar-28-tft-lcd-touch.html)

And followed excellent instructions from [Pete's soapbox](http://blog.ylett.com/2017/01/playing-with-jaycar-28-tft-lcd-touch.html)

Especially this...

_Fix the hardware identifier_
1. _This is the bit that took a while to figure out._
2. _For reference, the serial number on my TFT LCD is: QR4 5265S01 G3/2 TP28017._
3. _Locate line 60:  uint16_t identifier = tft.readID();_
4. _Change it to:  uint16_t identifier = 0x9341;_
5. _Or, equivalently, change line 84 to: tft.begin(0x9341);_


And added enhanced GFX library with fonts from [here](https://github.com/Bodmer/Adafruit_GFX_plus)


