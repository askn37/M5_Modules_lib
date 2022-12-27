/**
 * @file M5_GPS_LatLon.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <SoftwareUART.h>
#include <GPS_RMC.h>
SoftwareUART_Class GPS = {PIN_PA2, PIN_PA3};
GPS_RMC_Class GPS_RMC;

/* CASIC マルチモード衛星航法受信機プロトコル仕様 */
/* https://www.icofchina.com/d/file/xiazai/2017-05-02/ea0cdd3d81eeebcc657b5dbca80925ee.pdf */

const char _PCAS01_96[]   PROGMEM = "$PCAS01,1*1D"; /* 9600bps */
const char _PCAS01_384[]  PROGMEM = "$PCAS01,3*1F"; /* 38400bps */
const char _PCAS01_1152[] PROGMEM = "$PCAS01,5*19"; /* 115200bps */

const char _PCAS02_MIN[]  PROGMEM = "$PCAS02,1000*2E"; /* 1000ms=1Hz */
const char _PCAS02_HIGH[] PROGMEM = "$PCAS02,200*1D";  /* 200ms=5Hz */

/* x8 column (sec/print) : GGA,GLL,GSA,GSV,RMC,VTG,ZDA,TXT */
const char _PCAS03_STOP[] PROGMEM = "$PCAS03,0,0,0,0,0,0,0,0*04"; /* all stop */
const char _PCAS03_MIN[]  PROGMEM = "$PCAS03,0,0,0,0,1,0,0,0*03"; /* RMC only */
const char _PCAS03_MAX[]  PROGMEM = "$PCAS03,1,1,1,1,1,1,1,1*02"; /* Full */

const char _PCAS04_MIN[]  PROGMEM = "$PCAS04,1*18"; /* GPS */
const char _PCAS04_MID[]  PROGMEM = "$PCAS04,5*1C"; /* GPS+GLONASS */
const char _PCAS04_MAX[]  PROGMEM = "$PCAS04,7*1E"; /* GPS+BDS+GLONASS */

const char _PCAS12_60[]   PROGMEM = "$PCAS12,60*28"; /* Going Standby 60sec (Firmware V4.2.0.3) */

/* NMEA Checksum Calculator */
/* https://nmeachecksum.eqth.net */

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  GPS.begin(9600);
  // GPS.println(P(_PCAS01_1152));
  // GPS.end();
  // delay(100);
  // GPS.begin(115200);
  GPS.println(P(_PCAS02_MIN));
  GPS.println(P(_PCAS03_MIN));
  GPS.println(P(_PCAS04_MIN));
}

void loop (void) {
  char buff[1024];
  size_t length = GPS.readBytes(&buff, sizeof(buff));
  if (0 < length && '$' == buff[0]) {
    if (GPS_RMC.update(&buff, length)) {
      digitalWrite(LED_BUILTIN, TOGGLE);
      gpsdata_t gpsdata = GPS_RMC.getData();
      if (gpsdata.update.stamp) {
        Serial.print(F("date=")).print(gpsdata.stamp.date, ZHEX, 8);
        Serial.print('_');
        Serial.print(gpsdata.stamp.time, ZHEX, 6);
        if (gpsdata.update.coordinate) {
          Serial.print(F(",lat="));
          Serial.print(gpsdata.coordinate.latitude / 6000000.0, 7);
          Serial.print(':');
          Serial.print(gpsdata.coordinate.latitude / 6000000.0, 7);
          Serial.print(F(",lon="));
          Serial.print(gpsdata.coordinate.longitude / 6000000.0, 7);
          Serial.print(':');
          Serial.println(gpsdata.coordinate.longitude / 6000000.0 - 100.0, 7);
        }
        else {
          Serial.println(F(",lat=nan,lon=nan"));
        }
        return;
      }
    }
    Serial.println(F("date=nan,lat=nan,lon=nan"));
  }
}

// end of code
