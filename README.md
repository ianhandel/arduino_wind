# arduino_wind
Arduino sketch and schematics for anemometer with TFT display and NMEA 0183 output

Uses generic 2.8 TFT

I used Jaycar custom libraries from [here](http://blog.ylett.com/2017/01/playing-with-jaycar-28-tft-lcd-touch.html)

And followed excellent instructions from [Pete's soapbox](http://blog.ylett.com/2017/01/playing-with-jaycar-28-tft-lcd-touch.html)

Especially this...

_4. Fix the hardware identifier_

_This is the bit that took a while to figure out._

_For reference, the serial number on my TFT LCD is: QR4 5265S01 G3/2 TP28017._

_Locate line 60:  uint16_t identifier = tft.readID();_

_Change it to:  uint16_t identifier = 0x9341;_

_Or, equivalently, change line 84 to: tft.begin(0x9341);_





