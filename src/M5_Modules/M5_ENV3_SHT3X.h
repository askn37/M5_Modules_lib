/**
 * @file M5_ENV3_SHT3X.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <api/TWIM.h>    /* Wire methods */
#include <api/btools.h>  /* crc8_dallas() */
#include <api/capsule.h> /* _CAPS16() */
#include <api/delay_busywait.h>

#define ENVS_SHT3X_ADDR 0x44

class ENVS_SHT3X {
private:
  TWIM_Class TWIC;
  float temperature, humidity;

public:

  inline ENVS_SHT3X (TWIM_Class _twim) : TWIC(_twim) {}

  bool initialize (void) {
    TWIC.stop();
    return TWIC.start(ENVS_SHT3X_ADDR).is_ack();
  }

  bool update (void) {
    struct {
      int16_t temp;
      uint8_t temp_crc;
      uint16_t humi;
      uint8_t humi_crc;
    } _data;
    TWIC.stop();
    if (sizeof(_data) != TWIC
            .start(ENVS_SHT3X_ADDR)
      .send(0x2C).send(0x06) /* Clock stretching and High Repeatability */
      .request(ENVS_SHT3X_ADDR, sizeof(_data))
      .read((uint8_t*)&_data, sizeof(_data))) return false;
    // Serial.print(F("DATA=")).printHex(&_data, sizeof(_data), ':').ln();
    if (crc8_dallas((uint8_t*)&_data, 3)
     || crc8_dallas((uint8_t*)&_data + 3, 3)) return false;
    temperature = ((bswap16(_data.temp) * 175.0) / 65535.0) - 45.0;
    humidity    = ((bswap16(_data.humi) * 100.0) / 65535.0);
    return true;
  }

  float getTemperature (void) {
    return temperature;
  }

  float getHumidity (void) {
    return humidity;
  }

  // WBGT(InDoor) = 0.725*Ta + 0.0368*RH + 0.00364*Ta*RH - 3.246
  float getWBGTinDoor (void) {
    return 0.725 * temperature + 0.0368 * humidity + 0.00364 * temperature * humidity - 3.246;
  }

  // WBGT(OutDoor) = 0.735*Ta + 0.0374*RH + 0.00292*Ta*RH - 4.064
  float getWBGToutDoor (void) {
    return 0.735 * temperature + 0.0374 * humidity + 0.00292 * temperature * humidity - 4.064;
  }
};

// end of code

