/*
 * Francubo UTCkloc Arduino Sketch (UK Format: YYYY-MM-DD)
 * 
 * | 0          | 0 -> RX          |  PA11  |                 | EIC/EXTINT[11] ADC/AIN[19]           PTC/X[3] *SERCOM0/PAD[3]  SERCOM2/PAD[3]  TCC0/WO[3]  TCC1/WO[1]
 * | 1          | 1 <- TX          |  PA10  |                 | EIC/EXTINT[10] ADC/AIN[18]           PTC/X[2] *SERCOM0/PAD[2]                  TCC0/WO[2]  TCC1/WO[0]
 * | 2          | 2                |  PA14  |                 | EIC/EXTINT[14]                                 SERCOM2/PAD[2]  SERCOM4/PAD[2]  TC3/WO[0]   TCC0/WO[4]
 * | 3          | ~3               |  PA09  |                 | EIC/EXTINT[9]  ADC/AIN[17]           PTC/X[1]  SERCOM0/PAD[1]  SERCOM2/PAD[1] *TCC0/WO[1]  TCC1/WO[3]
 * | 4          | ~4               |  PA08  |                 | EIC/NMI        ADC/AIN[16]           PTC/X[0]  SERCOM0/PAD[0]  SERCOM2/PAD[0] *TCC0/WO[0]  TCC1/WO[2]
 * | 5          | ~5               |  PA15  |                 | EIC/EXTINT[15]                                 SERCOM2/PAD[3]  SERCOM4/PAD[3] *TC3/WO[1]   TCC0/WO[5]
 * | 6          | ~6               |  PA20  | UP_BT           | EIC/EXTINT[4]                        PTC/X[8]  SERCOM5/PAD[2]  SERCOM3/PAD[2]             *TCC0/WO[6]
 * | 7          | 7                |  PA21  |                 | EIC/EXTINT[5]                        PTC/X[9]  SERCOM5/PAD[3]  SERCOM3/PAD[3]              TCC0/WO[7]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 8          | ~8               |  PA06  |                 | EIC/EXTINT[6]  ADC/AIN[6]  AC/AIN[2] PTC/Y[4]  SERCOM0/PAD[2]                 *TCC1/WO[0]
 * | 9          | ~9               |  PA07  |                 | EIC/EXTINT[7]  ADC/AIN[7]  AC/AIN[3] PTC/Y[5]  SERCOM0/PAD[3]                 *TCC1/WO[1]
 * | 10         | ~10              |  PA18  | SET_BT          | EIC/EXTINT[2]                        PTC/X[6] +SERCOM1/PAD[2]  SERCOM3/PAD[2] *TC3/WO[0]    TCC0/WO[2]
 * | 11         | ~11              |  PA16  |                 | EIC/EXTINT[0]                        PTC/X[4] +SERCOM1/PAD[0]  SERCOM3/PAD[0] *TCC2/WO[0]   TCC0/WO[6]
 * | 12         | ~12              |  PA19  | DN_BT           | EIC/EXTINT[3]                        PTC/X[7] +SERCOM1/PAD[3]  SERCOM3/PAD[3]  TC3/WO[1]   *TCC0/WO[3]
 * | 13         | ~13              |  PA17  | LED             | EIC/EXTINT[1]                        PTC/X[5] +SERCOM1/PAD[1]
*/

#include "SAM32WiFiEsp.h"
#include "WiFiEspUdp.h"
#include <RTCZero.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"
#include <svm30.h>

#define DEBUG    0
#define AL_ADDR 0x48

char ssid[] = "WifiName";            // your network SSID (name)
char pass[] = "WifiPass";        // your network password

int status = WL_IDLE_STATUS;     // the Wifi radio's status

char timeServer[] = "time.nist.gov";  // NTP server
unsigned int localPort = 2390;        // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;  // NTP timestamp is in the first 48 bytes of the message
const int UDP_TIMEOUT = 2000;    // timeout in miliseconds to wait for an UDP packet to arrive

byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

int prevHours = 0;
int prevMinutes = 0;
int timezone = +2;
int UP_BT = 6;
int DN_BT = 12;
int SET_BT = 10;
int mode = 0;
int max_mode = 2;
// Settings for light sensitivity
// Possible values: .125, .25, 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
float gain = .125;

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light.
int time = 100;
long luxVal = 0;
// A UDP instance to let us send and receive packets over UDP
WiFiEspUDP Udp;
RTCZero rtc;
SVM30 svm;

Adafruit_AlphaNum4 ROW_1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 ROW_1_2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 ROW_2 = Adafruit_AlphaNum4();

SparkFun_Ambient_Light light(AL_ADDR);


void setup()
{
  // initialize pins
  pinMode(UP_BT,INPUT);
  pinMode(DN_BT,INPUT);
  pinMode(SET_BT,INPUT);

  // initialize serial for debugging
  if(DEBUG) SerialUSB.begin(115200);

  // initialize serial for ESP module
  Serial1.begin(115200);

  // initialize Wire for Display
  ROW_1.begin(0x74);  // pass in the address
  ROW_1_2.begin(0x72);
  ROW_2.begin(0x71);

  ROW_1.clear();
  ROW_1_2.clear();
  ROW_2.clear();

  ROW_1.writeDisplay();
  ROW_1_2.writeDisplay();
  ROW_2.writeDisplay();
  // initialize RTC
  rtc.begin();

  // initialize temp sensor
  svm.begin();
  // TODO decrease max mode possible
  if (svm.probe() == false) max_mode = 1;

  // initialize ESP module
  if(!light.begin() && DEBUG)
    SerialUSB.println("No Light Sensor!"); // initialize and check light module
  light.setGain(gain);
  light.setIntegTime(time);

  UpdateBrightness();
  String Bootscreen = " FRANCUBO    BOOTING   ";
  print2display(Bootscreen, false);

  WiFi.init(&Serial1);
  // check for the presence of the WiFi Module
  if (WiFi.status() == WL_NO_SHIELD) {
    if(DEBUG) SerialUSB.println("WiFi shield not present");
    Bootscreen = "WIFI ERROR";
    print2display(Bootscreen, false);
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    if(DEBUG) SerialUSB.print("Attempting to connect to WPA SSID: ");
    if(DEBUG) SerialUSB.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    Bootscreen = "  CONNECTED";
    print2display(Bootscreen, false);
    delay(400);
  }

  // you're connected now, so print out the data
  if(DEBUG) SerialUSB.println("You're connected to the network");
  Bootscreen = "GETTING TIME";
  print2display(Bootscreen, false);
  UpdateTime(timeServer);
  prevHours = rtc.getHours();
  prevMinutes = rtc.getMinutes();

}

void loop()
{
    if(!digitalRead(UP_BT)){
      if(mode < max_mode) mode++;
      else mode = 0;
      show_newmode(UP_BT);
    }
    if(!digitalRead(DN_BT)){
      if(mode > 0) mode--;
      else mode = max_mode;
      show_newmode(DN_BT);
    }
    switch (mode){
      case 0:
        ShowUTC();
        break;
      case 1:
        ShowEpoch();
        break;
      case 2:
        ShowTemp();
        break;
    }
    UpdateBrightness();
}

// send an NTP request to the time server at the given address
void sendNTPpacket(char *ntpSrv){

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(ntpSrv, 123); //NTP requests are to port 123

  Udp.write(packetBuffer, NTP_PACKET_SIZE);

  Udp.endPacket();
}



void print2digits(int number) {
  if (number < 10 && DEBUG) {
    SerialUSB.print("0");
  }
  if(DEBUG) SerialUSB.print(number);
  else return;
}

void print2display(String TimeText, bool dot){
  Wire.setClock(200000);
  if(TimeText.length() > 24) return;
  char displaybuffer[24] = {' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' '};
  for(int i=0;i<TimeText.length();i++){
    displaybuffer[i] = TimeText[i];
  }
  for(int i=TimeText.length();i<24-TimeText.length();i++){
    displaybuffer[i] = ' ';
  }
  for(int i=0;i<8;i++){
    ROW_1.writeDigitAscii(7-i, displaybuffer[i]);
    ROW_2.writeDigitAscii(7-i, displaybuffer[12+i]);
    ROW_1_2.writeDigitAscii(3-(i/2), displaybuffer[11-(i/2)]);
    ROW_1_2.writeDigitAscii(7-(i/2), displaybuffer[20+(i/2)]);
  }
  if(dot){
   ROW_2.writeDigitAscii(0,displaybuffer[19],true);
  }

  ROW_1.writeDisplay();
  ROW_2.writeDisplay();
  ROW_1_2.writeDisplay();
}


void UpdateTime(char server[]){
  Udp.begin(localPort);
  sendNTPpacket(server); // send an NTP packet to a time server
  unsigned long startMs = millis();

  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}
  if (Udp.parsePacket() && (millis() - startMs) < UDP_TIMEOUT) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    int localtimezone = (60 * 60 * timezone);
    unsigned long epoch = secsSince1900 - seventyYears + localtimezone;
    if(DEBUG) SerialUSB.println("Epoch got: ");
    if(DEBUG) SerialUSB.println(epoch);
    rtc.setEpoch(epoch);
  }
  else{
    Udp.stop();
    if (server != "time-a.timefreq.bldrdoc.gov"){
      if(DEBUG) SerialUSB.println("Error getting time");
      UpdateTime("time-a.timefreq.bldrdoc.gov");
    }

  }
  Udp.stop();
}


void UpdateBrightness(){

  luxVal = light.readLight();
  int brightness = map(sqrt(luxVal),0, 25, 0, 16);
  ROW_1.setBrightness(brightness);
  ROW_2.setBrightness(brightness);
  ROW_1_2.setBrightness(brightness);
  }

void ShowUTC(){

    TimeText += Millennium;
    TimeText += rtc.getYear();
    TimeText += "-";

    int month = rtc.getMonth();
    if(month < 10) TimeText += 0;
    TimeText += month;
    TimeText += "-";
    
    int day = rtc.getDay();
    String TimeText = "";
    String Millennium = "20";
    String CommonEra = "AC";
    String Timezone = "+2";
    if(day < 10) TimeText += 0;
    TimeText += day;
    TimeText += CommonEra;

    int hours = rtc.getHours();
    if(hours< 10) TimeText += 0;
    if(prevHours != hours){
      UpdateTime(timeServer);
      prevHours = hours;
    }
    TimeText += hours;
    TimeText += ":";

    int minutes = rtc.getMinutes();
    if(minutes < 10) TimeText += 0;
    if(prevMinutes != minutes){
      //UpdateBrightness();
      prevMinutes = minutes;
    }
    TimeText += minutes;
    TimeText += ":";

    int seconds = rtc.getSeconds();
    if(seconds < 10) TimeText += 0;
    TimeText += seconds;

    int microseconds = (millis() % 998 + 1)/10;
    if(microseconds < 10) TimeText += 0;
    TimeText += microseconds;
    TimeText += Timezone;
    //SerialUSB.println(TimeText);
    print2display(TimeText, true);
}

void ShowEpoch(){

    print2display("             " + String(rtc.getEpoch()),false);
    int hours = rtc.getHours();
    if(prevHours != hours){
      UpdateTime(timeServer);
      prevHours = hours;
    }
  }


void ShowTemp(){
  struct svm_values v;
  if (! svm.GetValues(&v)){
    print2display("No Temp Sensor", false);
    delay(1000);
  }
  String hum_sensor = String(int(v.humidity/10));
  String temp_sensor = String(int(v.temperature/10));
  String SenseText = "TEMP: " + temp_sensor + " C" + "HUM:  " + hum_sensor;
  char displaybuffer[24] = {' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' '};
  for(int i=0;i<SenseText.length();i++){
    displaybuffer[i] = SenseText[i];
  }
  for(int i=SenseText.length();i<24-SenseText.length();i++){
    displaybuffer[i] = ' ';
  }
  for(int i=0;i<8;i++){
    ROW_1.writeDigitAscii(7-i, displaybuffer[i]);
    ROW_2.writeDigitAscii(7-i, displaybuffer[12+i]);
    ROW_1_2.writeDigitAscii(3-(i/2), displaybuffer[11-(i/2)]);
    ROW_1_2.writeDigitAscii(7-(i/2), displaybuffer[20+(i/2)]);
  }
   ROW_2.writeDigitAscii(0,displaybuffer[19],true);
   ROW_1.writeDigitAscii(0,displaybuffer[7],true);

  ROW_1.writeDisplay();
  ROW_2.writeDisplay();
  ROW_1_2.writeDisplay();

}

void show_newmode(int Button){
    unsigned long timeNow = millis();
    boolean stillPressed = true;
    while(millis() < timeNow + 3000) {
      if(!digitalRead(UP_BT) && !stillPressed){
        if(mode < max_mode) mode++;
        else mode = 0;
        timeNow = millis();
        stillPressed = true;
        Button = UP_BT;
      }
      if(!digitalRead(DN_BT) && !stillPressed){
        if(mode > 0) mode--;
        else mode = max_mode;
        timeNow = millis();
        stillPressed = true;
        Button = DN_BT;
      }
      if(digitalRead(Button)) stillPressed = false;
      if (!digitalRead(SET_BT)) break;
      switch (mode){
        case 0:
          print2display(" UTC  CLOCK", false);
          break;
        case 1:
          print2display("   EPOCH     ",false);
          break;
        case 2:
          print2display("TEMPERATURE",false);
          break;
      }
    }
}
