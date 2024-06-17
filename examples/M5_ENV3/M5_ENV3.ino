/**
 * @file M5_ENV3.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-06
 *
 * @copyright Copyright (c) 2024 askn37 at github.com
 *
 */
// MIT License : https://askn37.github.io/LICENSE.html

#include <M5_Modules.h>

ENVS_QMP6988 ENV3_QMP = {Wire};
ENVS_SHT3X   ENV3_SHT = {Wire};

// #define APPEND_NCIR

#ifdef APPEND_NCIR
NCIR_MLX90614_Class NCIR = {Wire};
#endif

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  Serial.begin(CONSOLE_BAUD).println(F("\r<startup>"));

  /* speed selection : TWI_SM Only */
  Wire.initiate(TWI_SM, false);

  if (!ENV3_QMP.initialize()) {
    Serial.println(F("*** NACK ENVS_QMP6988"));
    return;
  }
  Serial.print(F("Found ENVS_QMP6988 address=0x"))
  .println(ENV3_QMP.current_addr(), HEX);

#ifdef APPEND_NCIR
  if (!NCIR.initialize()) {
    Serial.println(F("*** NACK NCIR_MLX90614"));
    return;
  }
#endif

  /* generate periodic */
  loop_until_bit_is_clear(RTC_PITSTATUS, RTC_CTRLBUSY_bp);
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC16384_gc;
}

EMPTY_INTERRUPT(RTC_PIT_vect);

void loop (void) {
  Serial.flush();
  RTC_PITINTFLAGS = RTC_PI_bm;
  sleep_cpu();
  digitalWrite(LED_BUILTIN, TOGGLE);

  if (
#ifdef APPEND_NCIR
    ENV3_QMP.update() && ENV3_SHT.update() && NCIR.update()
#else
    ENV3_QMP.update() && ENV3_SHT.update()
#endif
  ){
    /* QMP6988 */
    float temp1 = ENV3_QMP.getTemperature();
    float press = ENV3_QMP.getPressure();

    /* SHT3X */
    float temp2 = ENV3_SHT.getTemperature();
    float humi  = ENV3_SHT.getHumidity();
    float wbgti = ENV3_SHT.getWBGTinDoor();
    float wbgto = ENV3_SHT.getWBGToutDoor();
#ifdef APPEND_NCIR

    /* MLX90614 */
    float temp3 = NCIR.getAmbientTemperature();
    float temp4 = NCIR.getObjectTemperature();
#endif

    Serial
    .print(F("温度1=")).print(temp1).print(F("C:")).print(temp1)
    .print(F(",温度2=")).print(temp2).print(F("C:")).print(temp2)
#ifdef APPEND_NCIR
    .print(F(",温度3=")).print(temp3).print(F("C:")).print(temp3)
    .print(F(",温度4=")).print(temp4).print(F("C:")).print(temp4)
#endif
    .print(F(",WBGT(屋内)=")).print(wbgti).print(F("C:")).print(wbgti)
    .print(F(",WBGT(屋外)=")).print(wbgto).print(F("C:")).print(wbgto)
    .print(F(",湿度=")).print(humi).print(F("%PH:")).print(humi)
    .print(F(",気圧=")).print(press / 100.0).print(F("hPa(off900):")).print(press / 100.0 - 900).ln();
  }
  else {
    Serial.println(F("FAILD"));
  }
  // RTC_PITINTCTRL = 0;
}

// end of code
