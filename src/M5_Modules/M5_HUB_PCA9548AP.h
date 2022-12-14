/**
 * @file M5_HUB_PCA9548AP.h
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

#define HUB_PCA9548AP_ADDR_0 0x70
#define HUB_PCA9548AP_ADDR_1 0x71
#define HUB_PCA9548AP_ADDR_2 0x72
#define HUB_PCA9548AP_ADDR_3 0x73
#define HUB_PCA9548AP_ADDR_4 0x74
#define HUB_PCA9548AP_ADDR_5 0x75
#define HUB_PCA9548AP_ADDR_6 0x76
#define HUB_PCA9548AP_ADDR_7 0x77

class HUB_PCA9548AP_Class {
private:
  TWIM_Class TWIC;
  uint8_t _addr;

public:
  inline HUB_PCA9548AP_Class (TWIM_Class _twim) : TWIC(_twim), _addr(HUB_PCA9548AP_ADDR_0) {}
  inline HUB_PCA9548AP_Class (TWIM_Class _twim, const uint8_t addr) : TWIC(_twim), _addr(addr) {}

  uint8_t get (void) {
      return TWIC.request(_addr, 1).read();
  }

  bool set (const uint8_t _var) {
      return TWIC.start(_addr, 1).write(_var);
  }
};

// end of code
