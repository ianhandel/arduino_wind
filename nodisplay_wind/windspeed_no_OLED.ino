
// Modify write to HEX crc output
// Remove OLED


#include <SPI.h>
#include <Wire.h>


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
#define WINDPIN 2
#define WINDPIN_GND 4
#define DE_PIN 7
const float KNOTS_PER_HZ = 2.17244;
const float MILLIS_PER_SEC = 1000;
const float AVERAGE_INTERVALS = 5;
const long NOWIND_TIME = 5;
const int SPEEDS_STORED = 80;
const int LOOP_DELAY = 250;

volatile long now = millis();
volatile float MA_interval = 1000;
volatile float windspeed = 0;
volatile unsigned long speeds[SPEEDS_STORED] = {0};
volatile long index = 0;
volatile float gust = 0;
volatile float max_gust = 0;
volatile int write_index = 0;

void setup() {
  Serial.begin(4800);
  pinMode(WINDPIN, INPUT_PULLUP);
  pinMode(WINDPIN_GND, OUTPUT);
  pinMode(DE_PIN, OUTPUT);
  digitalWrite(WINDPIN_GND, LOW);
  digitalWrite(DE_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(WINDPIN), pulse, FALLING);
  now = millis();
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

