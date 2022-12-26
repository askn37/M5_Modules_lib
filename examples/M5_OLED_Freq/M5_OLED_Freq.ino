/**
 * @file M5_OLED_Freq.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <M5_Modules.h>
OLED_SH1107_Class OLED = {Wire, moderndos_8x16};

/* 目標周波数 : 基準 : 指定値 */
// 33554432 : 32 : 14 (256*256*512, 1S=(F>>25))
// 29491200 : 28 : 15 (115200*256)
// 24076800 : 24 : 3 (115200*209)
// 23040000 : 24 : -11 (115200*200)
// 22118400 : 24 : -32 (115200*192)
// 20044800 : 20 : 3 (115200*174)
// 16012800 : 16 : 3 (115200*139)
// 16777216 : 16 : 15 (256^3, 1S=(F>>24))
// 14745600 : 16 : -26 (115200*128)
// 24:31 -> 27MHz 24:-32 -> 22MHz
// 32:31 -> 36MHz 32:-32 -> 29MHz
#define OSCHFTUNE -11

void setup (void) {
  CLKCTRL_OSCHFTUNE = OSCHFTUNE;

  Wire.initiate(TWI_SM, false);
  OLED.clear(false);

  /* TCB0とTCB1の捕獲事象に接続 */
  /* 0.5Hz で捕獲 */
  EVSYS_CHANNEL4 = EVSYS_CHANNEL4_RTC_OVF_gc;
  EVSYS_USERTCB0CAPT = EVSYS_USER_CHANNEL4_gc;
  EVSYS_USERTCB1CAPT = EVSYS_USER_CHANNEL4_gc;

  /* ch5=TCB0溢れ*/
  /* TCB1の計数事象に接続 */
  EVSYS_CHANNEL5 = EVSYS_CHANNEL5_TCB0_OVF_gc;
  EVSYS_USERTCB1COUNT = EVSYS_USER_CHANNEL5_gc;

  /* TCB1は計数捕獲周波数測定動作かつ連結上位 */
  /* クロック元は CLK_PER */
  TCB1_EVCTRL = TCB_CAPTEI_bm;
  TCB1_CTRLB = TCB_CNTMODE_FRQ_gc;
  TCB1_CTRLA = TCB_CASCADE_bm | TCB_ENABLE_bm | TCB_CLKSEL_EVENT_gc;

  /* TCB0は計数捕獲周波数測定動作かつ連結下位 */
  /* CLK元は主クロック（F_CPU）*/
  TCB0_EVCTRL = TCB_CAPTEI_bm;
  TCB0_CTRLB = TCB_CNTMODE_FRQ_gc;
  TCB0_CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;

  loop_until_bit_is_clear(RTC_STATUS, RTC_CTRLABUSY_bp);
  RTC_CTRLA = RTC_RTCEN_bm;
}

void loop (void) {
  uint32_t _count;
  loop_until_bit_is_set(TCB1_STATUS, TCB_CAPT_bp);
  _CAPS32(_count)->words[0] = TCB0_CCMP;
  _CAPS32(_count)->words[1] = TCB1_CCMP;
  OLED.setPosition(0, 0).print(_count, DEC, 12);
}

// end of code
