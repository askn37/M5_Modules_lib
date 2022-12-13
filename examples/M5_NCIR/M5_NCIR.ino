/**
 * @file M5_NCIR.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <M5_Modules.h>

NCIR_MLX90614_Class NCIR = {Wire};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* speed selection : TWI_SM Only */
  Wire.initiate(TWI_SM, false);

  Serial.print(F("F_CPU=")).print(F_CPU, DEC);
  Serial.print(F(",MBAUD=")).println(Wire.is_baud(), DEC);

  /* option : I2C HUB PCA9548A (0x70) selected channels (all) */
  // Wire.start(0x70).send(0xFF).stop();

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

ISR(RTC_PIT_vect) { RTC_PITINTFLAGS = RTC_PI_bm; }

void loop (void) {
  Serial.flush();
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
