/**
 * @file M5_TOF.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <M5_Modules.h>

TOF_VL53L0X_Class TOF = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  Wire.initiate(TWI_SM, false);

  if ( !TOF.initialize() ) {
    Serial.println(F("nodevice"));
    for (;;);
  }

  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC16384_gc;

  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
}

EMPTY_INTERRUPT(RTC_PIT_vect);

void loop (void) {
  digitalWrite(LED_BUILTIN, TOGGLE);

  if ( TOF.update() && TOF.getDistance() > 0 ) {
    Serial.print(F(",Dist=")).print(TOF.getDistance());
    Serial.print(':').println(TOF.getDistance());
  }
  else {
    Serial.println(F(",Dist=nan:nan"));
  }

  Serial.flush();
  RTC_PITINTFLAGS = RTC_PI_bm;
  sleep_cpu();
}

// end of code
