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

/* sensor device */
NCIR_MLX90614_Class NCIR = {Wire};

/* constructor and Load FontSet : 0x32-0x7F */
OLED_SH1107_Class OLED = {Wire, moderndos_8x16};

/* option : I2C HUB PCA9548A selected address (default) */
HUB_PCA9548AP_Class I2CHUB = {Wire, HUB_PCA9548AP_ADDR_0};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  /* speed selection : TWI_SM Only */
  Wire.initiate(TWI_SM, false);

  /* option : I2C HUB PCA9548A selected channels (all) */
  I2CHUB.set(0xFF);

  OLED.clear(false);
  OLED.setPosition(0, 0).print(F("AMB:"));
  OLED.setPosition(0, 1).print(F("OBJ:"));
  OLED.flush();

  /* generate periodic */
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC16384_gc;
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

EMPTY_INTERRUPT(RTC_PIT_vect);

void loop (void) {
  RTC_PITINTFLAGS = RTC_PI_bm;
  sleep_cpu();
  digitalWrite(LED_BUILTIN, TOGGLE);
  if (NCIR.update()) {
    float temp0 = NCIR.getAmbientTemperature();
    float temp1 = NCIR.getObjectTemperature();
    OLED.setPosition(5, 0).print(temp0).print(F("\x7F\x43"));
    OLED.setPosition(5, 1).print(temp1).print(F("\x7F\x43"));
  }
  else {
    OLED.setPosition(5, 0).print(F("FAIL"));
  }
  OLED.flush();
}

// end of code
