/**
 * @file M5_NCIR_MLX90614.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <math.h>
#include <api/TWIM.h>    /* Wire methods */
#include <api/btools.h>  /* crc8() */
#include <api/capsule.h> /* _CAPS16() */
#include <api/delay_busywait.h>

#define NCIR_MLX90614_ADDR 0x5A /* 7bit address */

/* Restore Config_1 */
// 0xAFF4; Dual or Middle Model
// 0x9FB4; Single Short Model

typedef enum NCIR_MLX90614_enum : uint8_t {
    NCIR_RAM_AMBIENT_gc    = 0x06U
  , NCIR_RAM_OBJECT_gc     = 0x07U
  , NCIR_RAM_OBJECT2_gc    = 0x08U
  , NCIR_ROM_EMISSIVITY_gc = 0x24U
  , NCIR_ROM_CONFIG_gc     = 0x25U
  , NCIR_ROM_IDNUM_gc      = 0x3CU
  , NCIR_REG_FLAGS_gc      = 0xF0U
  , NCIR_ENTER_SLEEP_gc    = 0xFFU
} NCIR_MLX90614_enum_t;

class NCIR_MLX90614_Class {
private:
  TWIM_Class TWIC;
  float _temperature0, _temperature1, _temperature2;

  bool SMBus_read (const uint8_t _register, uint16_t& _result) {
    uint8_t payload[6] = {
      (NCIR_MLX90614_ADDR << 1)
    , _register
    , (NCIR_MLX90614_ADDR << 1) + 1
    };
    if (3 != TWIC
      .start(NCIR_MLX90614_ADDR)
      .send(_register)
      .request(NCIR_MLX90614_ADDR, 3)
      .read(&payload[3], 3)) return false;
    _result = _CAPS16(payload[3])->word;
    return crc8(&payload, 6) == 0;
  }

  bool SMBus_write (const uint8_t _register, uint16_t _value) {
    struct {
      uint8_t  addr;
      uint8_t  reg;
      uint16_t data;
      uint8_t  pec;
    } payload;
    payload = {
      (NCIR_MLX90614_ADDR << 1)
    , _register
    , _value
    , 0
    };
    payload.pec = crc8((uint8_t*)&payload, 4);
    return 4 == TWIC
      .start(NCIR_MLX90614_ADDR, 4)
      .write((uint8_t*)&payload + 1, 4);
  }

public:
  inline NCIR_MLX90614_Class (TWIM_Class _twim) : TWIC(_twim) {}

  bool initialize (void) {
    return SMBus_write(NCIR_ROM_EMISSIVITY_gc, 0xFFFF);
  }

  bool getTemperature (const uint8_t _register, float &_temperature) {
    uint16_t _data;
    if (!SMBus_read(_register, _data)) return false;
    _temperature = _data * 0.02 - 273.15;
    return (_data >> 15) == 0;
  }

  bool update (void) {
    return getTemperature(NCIR_MLX90614_enum::NCIR_RAM_AMBIENT_gc, _temperature0)
        && getTemperature(NCIR_MLX90614_enum::NCIR_RAM_OBJECT_gc,  _temperature1)
        && getTemperature(NCIR_MLX90614_enum::NCIR_RAM_OBJECT2_gc, _temperature2);
  }

  float getAmbientTemperature (void) { return _temperature0; }
  float getObjectTemperature (void) { return _temperature1; }
  float getObjectTemperature2 (void) { return _temperature2; }

  bool sleep (void) {
    uint8_t _sleep[] = {NCIR_MLX90614_enum::NCIR_ENTER_SLEEP_gc, 0xE8};
    return 2 == TWIC.start(NCIR_MLX90614_ADDR, 2).write(_sleep, sizeof(_sleep));
    /** after task hold lowpower mode **
      Wire.end();               // release TWI control
      digitalWrite(SDA, HIGH); pinMode(SDA, OUTPUT);
      digitalWrite(SCL, LOW);  pinMode(SCL, OUTPUT);
    */
    /** wakeup lowpower **
      digitalWrite(SCL, HIGH);  // hold SCL HIGH
      digitalWrite(SDA, LOW);   // hold SDA LOW
      delay(33);                // Wake Up delay 33ms
      pinMode(SCL, INPUT);      // release open-drain
      pinMode(SDA, INPUT);      // release open-drain
      Wire.begin();             // re-init I2C bus
      delay(250);               // First sampling delay 250ms
    */
  /** after task hold lowpower mode **
    Wire.end();               // release TWI control
    digitalWrite(SDA, HIGH); pinMode(SDA, OUTPUT);
    digitalWrite(SCL, LOW);  pinMode(SCL, OUTPUT);
  */
  /** wakeup lowpower **
    digitalWrite(SCL, HIGH);  // hold SCL HIGH
    digitalWrite(SDA, LOW);   // hold SDA LOW
    delay(33);                // Wake Up delay 33ms
    pinMode(SCL, INPUT);      // release open-drain
    pinMode(SDA, INPUT);      // release open-drain
    Wire.begin();             // re-init I2C bus
    delay(250);               // First sampling delay 250ms
  */
  }

};

// end of code
