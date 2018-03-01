#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Fonts/FreeMonoBold24pt7b.h>


// TOUCHSCREEN STUFF from TFT-LCD paint sketch
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

long wind = 0;

void setup(void) {
  tft.reset();
  uint16_t identifier = 0x9341;

  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  Serial.begin(9600);
}

void loop(void) {

  // Write something
  wind = random(0, 99);
  text(wind);
  delay(250);

  // Read screen
  TSPoint p = ts.getPoint();
   if (p.z > MINPRESSURE) {
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
    }

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}

void text(long wind) {
  tft.fillScreen(BLACK);
  tft.setCursor(20, 180);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);
  tft.setFont(&FreeMonoBold24pt7b);
  tft.println(wind);
}

