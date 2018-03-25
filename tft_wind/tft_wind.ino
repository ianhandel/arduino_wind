#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Fonts/FreeMonoBold24pt7b.h>



//WINDSPEED and NMEA stuff
// stuff for NMEA
const byte buff_len = 90;
char CRCbuffer[buff_len];

// create pre-dfined strings to control flexible output formatting
String sp = " ";
String delim = ",";
String splat = "*";
String reference = "R";
String units = "N";
String valid = "A";
String msg = "";

#define BOUNCE 25
#define WINDPIN 18
#define WINDPIN_GND 19

const float KNOTS_PER_HZ = 2.17244;
const float MILLIS_PER_SEC = 1000;
const float AVERAGE_INTERVALS = 5;
const long NOWIND_TIME = 5;
const int SPEEDS_STORED = 80;
const int LOOP_DELAY = 250;

volatile long now = millis();
volatile float MA_interval = 1000;
volatile float windspeed = 0;
volatile int num_old = 0;
volatile unsigned long speeds[SPEEDS_STORED] = {0};
volatile long index = 0;
volatile float gust = 0;
volatile float max_gust = 0;
volatile int write_index = 0;

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

void setup(void) {
  // tft stuff
  tft.reset();
  uint16_t identifier = 0x9341;

  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  //wind and NMEA
  Serial.begin(4800);
  pinMode(WINDPIN, INPUT_PULLUP);
  pinMode(WINDPIN_GND, OUTPUT);
  digitalWrite(WINDPIN_GND, LOW);
  attachInterrupt(digitalPinToInterrupt(WINDPIN), pulse, FALLING);
  now = millis();

  text(88);
  tft.fillScreen(BLACK);
}


void loop() {
  if((millis() - now) > (NOWIND_TIME * MILLIS_PER_SEC)){
    windspeed = 0;
  }

// store speeds and find gust speed and max gust
  speeds[index] = windspeed;
  index = (index + 1) % SPEEDS_STORED;
  gust = 0;
  for(int ii = 0; ii < SPEEDS_STORED; ii++){
    if(speeds[ii] > gust){
      gust = speeds[ii];
    }
  }
  if(gust > max_gust){
    max_gust = gust;
  }

  if(write_index++ == 0){
      // build msg
      // $--MWV,x.x,a,x.x,a*hh<CR><LF>
      char strAngle[8];
      char strSpeed[8];
      float angle = 0;
      String cmd = "$WIMWV";    // a command name
      dtostrf(angle, 4, 1, strAngle);   // format float value to string XX.X
      dtostrf(windspeed, 4, 1, strSpeed);
      msg = cmd + delim + strAngle + delim + reference + delim + strSpeed + delim + units + delim + valid + splat;
      outputMsg(msg); // print the entire message string, and append the CRC
  }
  write_index %= 4;

  
  // Write windspeed
  text(windspeed);


/*
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
*/



  
  delay(LOOP_DELAY);
}


// -----------------------------------------------------------------------
void pulse() {
  if (millis() - now > BOUNCE) {
    MA_interval = (MA_interval * (AVERAGE_INTERVALS - 1) + millis() - now) / AVERAGE_INTERVALS;
    windspeed = MILLIS_PER_SEC / MA_interval * KNOTS_PER_HZ;
    now = millis();
  }
}

// -----------------------------------------------------------------------
void outputMsg(String msg) {
  msg.toCharArray(CRCbuffer, sizeof(CRCbuffer)); // put complete string into CRCbuffer
  int crc = convertToCRC(CRCbuffer);

  Serial.print(msg);
  if (crc < 16) Serial.print("0"); // add leading 0
  Serial.println(crc, HEX);
}

// -----------------------------------------------------------------------
byte convertToCRC(char *buff) {
  // NMEA CRC: XOR each byte with previous for all chars between '$' and '*'
  char c;
  byte i;
  byte start_with = 0;
  byte end_with = 0;
  byte CRC = 0;

  for (i = 0; i < buff_len; i++) {
    c = buff[i];
    if (c == '$') {
      start_with = i;
    }
    if (c == '*') {
      end_with = i;
    }
  }
  
  for (i = start_with + 1; i < end_with; i++) { // XOR every character between '$' and '*'
      CRC = CRC ^ buff[i] ;  // compute CRC
    }

  return CRC;
  //based on code by Elimeléc López - July-19th-2013
}


void text(int num) {
  if (num != num_old) {
    tft.setCursor(20, 200);
    if (num_old < 10) {
      tft.setCursor(160, 200);
    }
    tft.setTextSize(5);
    tft.setFont(&FreeMonoBold24pt7b);
    tft.setTextColor(BLACK);
    tft.println(num_old);
  }
  tft.setCursor(20, 200);
  if (num < 10) {
    tft.setCursor(160, 200);
    }
  tft.setTextSize(5);
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(YELLOW);
  tft.println(num);
  num_old = num;
}

