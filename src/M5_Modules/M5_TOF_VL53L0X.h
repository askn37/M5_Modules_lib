/**
 * @file TOF_VL53L0X.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <api/TWIM.h>

#define TOF_VL53L0X_ADDR 0x29

class TOF_VL53L0X_Class {
private:
  TWIM_Class TWIC;
  uint16_t _AmbientCount, _SignalCount;
  int16_t _Distance;

public:
  inline TOF_VL53L0X_Class (TWIM_Class _twim) : TWIC(_twim) {}

  bool initialize (void) {
    uint16_t model_id;
    if ( !TWIC.scan(TOF_VL53L0X_ADDR) ) return false;
    if (2 != TWIC
      .start(TOF_VL53L0X_ADDR).send(0xc0)
      .request(TOF_VL53L0X_ADDR, 2)
      .read(&model_id, 2)
    ) return false;
    return model_id == 0xAAEE;
  }

  bool update (void) {
    /* wakeup device */
    delay_millis(1);
    if ( 2 != TWIC
      .start(TOF_VL53L0X_ADDR, 2)
      .send(0x00)
      .send(0x01)
      .write_size()
    ) return false;

    /* oneshot sampling delay */
    uint8_t _buff[12];
    uint8_t _count = 10;
    do {
      delay_millis(5);
      if ( 12 != TWIC
        .start(TOF_VL53L0X_ADDR)
        .send(0x14)
        .request(TOF_VL53L0X_ADDR, 12)
        .recv(_buff, 12)
        .read_size()
      ) return false;
      if (--_count == 0) return false;
    } while ((_buff[0] & 1) == 0);

    _AmbientCount = bswap16(_PTR16(_buff)[3]);
    _SignalCount  = bswap16(_PTR16(_buff)[4]);
    _Distance     = bswap16(_PTR16(_buff)[5]);

    /* undef limit : over limit */
    if (_Distance < 25) _Distance = 0;
    else if (_Distance > 8000) _Distance = -1;

    return true;
  }

  uint16_t getAmbientCount (void) { return _AmbientCount; }
  uint16_t getSignalCount (void) { return _SignalCount; }
  int16_t getDistance (void) { return _Distance; }

};

// end of code
