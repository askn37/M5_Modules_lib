/**
 * @file M5_OLED_Console.ino
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <M5_Modules.h>

/* constructor and Load FontSet : 0x32-0x7F */
OLED_SH1107_Class OLED = {Wire, moderndos_8x16};

/* Volatility External character user area : 0x10-0x1F */
static uint8_t extra_table[256] = {};

void setup (void) {
  pinMode(LED_BUILTIN, OUTPUT);
  #ifdef LED_BUILTIN_INVERT
  pinControlRegister(LED_BUILTIN) = PORT_INVEN_bm;
  #endif

  /* speed selection : TWI_SM TWI_FM TWI_FMP */
  Wire.initiate(TWI_SM, false);

  /* option : I2C HUB PCA9548A (0x70) selected channels (all) */
  Wire.start(0x70).send(0xFF).stop();

  /* custom pointer */
  OLED
    .clear()
    // .setFlip(false)
    // .setRevesible(true)
    .setExtTableInROM(moderndos_8x16) /* 0x80-0xFF External character bank */
    .setExtTableInRAM(extra_table)    /* 0x10-0x1F Volatility External character area */
  ;

  /* Volatility External character Setup */
  for (int i = 0; i < 16; i++) {
    OLED
      .setExtFont(&extra_table[i << 4], '@' + i, OLED_SET) /* Load ROM Font */
      .setExtFont(&extra_table[i << 4], '\x0', OLED_XOR)   /* Exor Mask */
    ;
  }

  /* fill the screen with a test pattern */
  // OLED.testPattern();

  /* generate character stroke weight */
  RTC_PITINTCTRL = RTC_PI_bm;
  RTC_PITCTRLA = RTC_PITEN_bm | RTC_PERIOD_CYC512_gc; /* 32768 DIV Number */
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

ISR(RTC_PIT_vect) { RTC_PITINTFLAGS = RTC_PI_bm; }

void loop (void) {
  for (int _c = 16; _c < 224; _c++) {
    OLED.write(_c);
    sleep_cpu(); /* comment out is max display speed */
    digitalWrite(LED_BUILTIN, TOGGLE);
  }
  OLED.println(F("Hello World!"));
}

// end of code
