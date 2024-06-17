/**
 * @file M5_NCIR.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <M5_Modules.h>

NCIR_MLX90614_Class NCIR = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* speed selection : TWI_SM Only */
  Wire.initiate(TWI_SM, false);

  Serial.print(F("F_CPU=")).print(F_CPU, DEC);

  if (!NCIR.initialize()) {
    Serial.println(F("NCIR notready"));
    Serial.flush();
    power_down();
  }

  /* generate periodic */
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC16384_gc;
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

EMPTY_INTERRUPT(RTC_PIT_vect);

void loop (void) {
  Serial.flush();
  RTC_PITINTFLAGS = RTC_PI_bm;
  sleep_cpu();
  digitalWrite(LED_BUILTIN, TOGGLE);
  if (NCIR.update()) {
    float temp0 = NCIR.getAmbientTemperature();
    float temp1 = NCIR.getObjectTemperature();
    Serial.print(F("Ambient=")).print(temp0).print(F("C:")).print(temp0);
    Serial.print(F(",Object=")).print(temp1).print(F("C:")).println(temp1);
  }
  else {
    Serial.println(F("FAILD"));
  }
}

// end of code
