/**
 * @file M5_GPS_NMEA.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-12
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <SoftwareUART.h>
SoftwareUART_Class GPS = {PIN_PA2, PIN_PA3};

/* CASIC マルチモード衛星航法受信機プロトコル仕様 */
/* https://www.icofchina.com/d/file/xiazai/2017-05-02/ea0cdd3d81eeebcc657b5dbca80925ee.pdf */

const char _PCAS01_96[]   PROGMEM = "$PCAS01,1*1D"; /* 9600bps */
const char _PCAS01_384[]  PROGMEM = "$PCAS01,3*1F"; /* 38400bps */
const char _PCAS01_1152[] PROGMEM = "$PCAS01,5*19"; /* 115200bps */

const char _PCAS02_MIN[]  PROGMEM = "$PCAS02,1000*2E"; /* 1000ms=1Hz */
const char _PCAS02_HIGH[] PROGMEM = "$PCAS02,200*1D";  /* 200ms=5Hz */

/* x8 column (sec/print) : GGA,GLL,GSA,GSV,RMC,VTG,ZDA,TXT */
const char _PCAS03_STOP[] PROGMEM = "$PCAS03,0,0,0,0,0,0,0,0*04"; /* all stop */
const char _PCAS03_MIN[]  PROGMEM = "$PCAS03,0,0,0,0,1,0,0,0*03"; /* nRMC only */
const char _PCAS03_MAX[]  PROGMEM = "$PCAS03,1,1,1,1,1,1,1,1*02"; /* Full */

const char _PCAS04_MIN[]  PROGMEM = "$PCAS04,1*18"; /* GPS */
const char _PCAS04_MID[]  PROGMEM = "$PCAS04,5*1C"; /* GPS+GLONASS */
const char _PCAS04_MAX[]  PROGMEM = "$PCAS04,7*1E"; /* GPS+BDS+GLONASS */

const char _PCAS12_60[]   PROGMEM = "$PCAS12,60*28"; /* Going Standby 60sec (Firmware V4.2.0.3) */

/* NMEA Checksum Calculator */
/* https://nmeachecksum.eqth.net */

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, TOGGLE);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  GPS.begin(9600);
  // GPS.println(P(_PCAS01_1152));
  // GPS.end();
  // delay(100);
  // GPS.begin(115200);
  GPS.println(P(_PCAS02_MIN)); /* Console Freq */
  GPS.println(P(_PCAS03_MAX)); /* Console Periodic */
  GPS.println(P(_PCAS04_MID)); /* Target System */
}

void loop (void) {
  size_t length;
  char buff[768];
  length = GPS.readBytes(&buff, sizeof(buff));
  if (length > 0 && '$' == buff[0]) {
    digitalWrite(LED_BUILTIN, TOGGLE);
    Serial.write(&buff, length);
    Serial.print(F("LEN=")).println(length);
  }
}

// end of code
