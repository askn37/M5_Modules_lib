/**
 * @file M5_ENV3_QMP6988.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-11-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <api/TWIM.h>    /* Wire methods */
#include <api/btools.h>  /* crc8() */
#include <api/capsule.h> /* _CAPS16() */
#include <api/delay_busywait.h>

#define ENVS_QMP6988_ADDR_L 0x70
#define ENVS_QMP6988_ADDR_H 0x56

#define ENVS_QMP6988_SAMPLE_MS 12

typedef enum ENVS_QMP6988_enum : uint8_t {
  ENVS_CHIP_ID         = 0x5C
, ENVS_CHIP_ENABLE     = 0x00
, ENVS_CHIP_RESET      = 0xE6
, ENVS_ROM_OTP         = 0xA0
, ENVS_ROM_CHIP_ID     = 0xD1
, ENVS_ROM_RESET       = 0xE0
, ENVS_REG_IIR_CNT     = 0xF1
, ENVS_REG_I2C_SET     = 0xF2
, ENVS_REG_DEVICE_STAT = 0xF3
, ENVS_REG_CTRL_MEAS   = 0xF4
, ENVS_REG_IO_SETUP    = 0xF5
, ENVS_ADC_PRESS       = 0xF7
, ENVS_ADC_TEMP        = 0xFA
} ENVS_QMP6988_enum_t;

struct ENVS_QMP6988_COE_K_t {
  float a0, a1, a2;
  float b00, bt1, bp1;
  float b11, bt2, bp2;
  float b12, b21, bp3;
};

struct ENVS_QMP6988_COE_OTP_t {
  uint8_t reserve;
  uint8_t b00_a0_ex;
  int16_t a2, a1, a0;
  int16_t bp3, b21, b12;
  int16_t bp2, b11, bp1;
  int16_t bt2, bt1, b00;
};

class ENVS_QMP6988 {
private:
  TWIM_Class TWIC;
  float _temperature, _pressure;
  uint8_t _addr = 0;
  ENVS_QMP6988_COE_K_t COE_K = {};

  void _next_delay (void) {
    delay_micros(5); /* next delay */
  }

  bool _device_check (uint8_t addr) {
    if (ENVS_CHIP_ID == TWIC.start(addr)
      .send(ENVS_ROM_CHIP_ID)
      .request(addr, 1).read()) _addr = addr;
    _next_delay();
    return _addr == addr;
  }

public:
  inline ENVS_QMP6988 (TWIM_Class _twim) : TWIC(_twim) {}

  inline uint8_t current_addr (void) { return _addr; }

  void reset (void) {
    TWIC.start(_addr, 2).send(ENVS_ROM_RESET).send(ENVS_CHIP_RESET);
    delay_micros(60); /* minimum 57 us */
    TWIC.start(_addr, 2).send(ENVS_ROM_RESET).send(ENVS_CHIP_ENABLE);
    delay_micros(300);
  }

  bool initialize (void) {
    if (!_device_check(ENVS_QMP6988_ADDR_H) &&
        !_device_check(ENVS_QMP6988_ADDR_L)) return false;
    /* Software Reset */
    reset();
    /* IIR filter co-efficient setting */
    TWIC.start(_addr).send(ENVS_REG_IIR_CNT).send(0b00000001);
    _next_delay();
    /* First Shot Measure */
    TWIC.start(_addr).send(ENVS_REG_CTRL_MEAS).send(0b01101101);
    _next_delay();
    ENVS_QMP6988_COE_OTP_t _COE_OTP = {};
    if (25 != TWIC.start(_addr)
      .send(ENVS_ROM_OTP)
      .request(_addr, 25)
      .reverse_read((uint8_t*)&_COE_OTP + 1, 25)) return false;
    _next_delay();
    // Serial.print(F("_COE_OTP=")).printHex(&_COE_OTP, sizeof(_COE_OTP), ':').ln();
    COE_K.a0  = (((int32_t)_COE_OTP.a0  << 16) + ((int32_t)(_COE_OTP.b00_a0_ex & 0x0F) << 12)) / 65536.0;
    COE_K.b00 = (((int32_t)_COE_OTP.b00 << 16) + ((int32_t)(_COE_OTP.b00_a0_ex & 0xF0) << 8))  / 65536.0;
    COE_K.a1  = -6.30E-03 + (4.30E-04 * _COE_OTP.a1 ) / 32767.0;
    COE_K.a2  = -1.90E-11 + (1.20E-10 * _COE_OTP.a2 ) / 32767.0;
    COE_K.bt1 = +1.00E-01 + (9.10E-02 * _COE_OTP.bt1) / 32767.0;
    COE_K.bt2 = +1.20E-08 + (1.20E-06 * _COE_OTP.bt2) / 32767.0;
    COE_K.bp1 = +3.30E-02 + (1.90E-02 * _COE_OTP.bp1) / 32767.0;
    COE_K.b11 = +2.10E-07 + (1.40E-07 * _COE_OTP.b11) / 32767.0;
    COE_K.bp2 = -6.30E-10 + (3.50E-10 * _COE_OTP.bp2) / 32767.0;
    COE_K.b12 = +2.90E-13 + (7.60E-13 * _COE_OTP.b12) / 32767.0;
    COE_K.b21 = +2.10E-15 + (1.20E-14 * _COE_OTP.b21) / 32767.0;
    COE_K.bp3 = -1.30E-16 + (7.90E-17 * _COE_OTP.bp3) / 32767.0;
    // Serial.print(F("COE_K.a0=")).println(COE_K.a0,   6);
    // Serial.print(F("COE_K.b00=")).println(COE_K.b00, 6);
    // Serial.print(F("COE_K.a1=")).println(COE_K.a1,   6);
    // Serial.print(F("COE_K.a2=")).println(COE_K.a2,   6);
    // Serial.print(F("COE_K.bt1=")).println(COE_K.bt1, 6);
    // Serial.print(F("COE_K.bt2=")).println(COE_K.bt2, 6);
    // Serial.print(F("COE_K.bp1=")).println(COE_K.bp1, 6);
    // Serial.print(F("COE_K.bp1=")).println(COE_K.bp1, 6);
    // Serial.print(F("COE_K.b11=")).println(COE_K.b11, 6);
    // Serial.print(F("COE_K.bp2=")).println(COE_K.bp2, 6);
    // Serial.print(F("COE_K.b12=")).println(COE_K.b12, 6);
    // Serial.print(F("COE_K.b21=")).println(COE_K.b21, 6);
    // Serial.print(F("COE_K.bp3=")).println(COE_K.bp3, 6);
    return true;
  }

  bool update (void) {
    struct {
      int32_t  Dt = 0;
      uint32_t Dp = 0;
    } _data;
    TWIC.start(_addr).send(ENVS_REG_CTRL_MEAS).send(0b01101101);
    delay_millis(ENVS_QMP6988_SAMPLE_MS);
    if (6 != TWIC.start(_addr)
      .send(ENVS_ADC_PRESS)
      .request(_addr, 6)
      .reverse_read((uint8_t*)&_data + 1, 6)) return false;
    _next_delay();
    // Serial.print(F("_COE_ADC=")).printHex(&_data, sizeof(_data), ':').ln();
    _data.Dt >>= 8;
    float Dt = _data.Dt - 8388608.0;
    _temperature = (COE_K.a0 + (COE_K.a1 * Dt) + (COE_K.a2 * (Dt * Dt))) / 256.0;
    // Serial.print(F("Tr=")).println(_temperature);
    float Dp = _data.Dp - 8388608.0;
    float Tr2 = _temperature * _temperature;
    float Dp2 = Dp * Dp;
    float Dp3 = Dp2 * Dp;
    // Serial.print(F("Dp=")).println(Dp);
    _pressure = COE_K.b00
      + (COE_K.bt1 * _temperature)
      + (COE_K.bp1 * Dp)
      + (COE_K.b11 * Dp)
      + (COE_K.bt2 * Tr2)
      + (COE_K.bp2 * Dp2)
      + (COE_K.b12 * Dp * Tr2)
      + (COE_K.b21 * Dp2 * _temperature)
      + (COE_K.bp3 * Dp3);
    return true;
  }

  float getTemperature (void) {
    return _temperature;
  }

  float getPressure (void) {
    return _pressure;
  }
};

// end of code
