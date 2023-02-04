/**
 * @file M5_OLED_SH1107.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <avr/pgmspace.h>
#include <api/TWIM.h>
#include <api/Print.h>
#include <api/String.h>
#include <FontSets.h>

#ifndef __OLED_Console_Logic_enum
typedef enum OLED_Console_Logic_enum : uint8_t {
    OLED_SET = 0
  , OLED_OR  = 1
  , OLED_AND = 2
  , OLED_XOR = 3
} OLED_Console_Logic_t;
#define __OLED_Console_Logic_enum
#endif

class OLED_SH1107_Class : public Print {
private:

  TWIM_Class TWIC;
  uint8_t* FontSet;
  uint8_t* ExtraROMSet = nullptr;
  uint8_t* ExtraRAMSet = nullptr;
  uint8_t _cx, _cy, _cz, _ct = 0x80;
  bool _console_mode;

  void set_page (uint8_t _page, uint8_t _len);

public:
  inline OLED_SH1107_Class (TWIM_Class _twim, const uint8_t _fontset[]) : TWIC(_twim), FontSet((uint8_t*)_fontset) {}

  OLED_SH1107_Class& disable (void);
  OLED_SH1107_Class& clear (bool console_mode = true);
  OLED_SH1107_Class& setFlip (bool flip_mode = true);
  OLED_SH1107_Class& setRevesible (bool reverse_mode = true);
  OLED_SH1107_Class& setContrast (uint8_t _contrast);
  OLED_SH1107_Class& setScroll (uint8_t _offset);
  OLED_SH1107_Class& setPosition (uint8_t _x, uint8_t _y);
  OLED_SH1107_Class& drawTestPattern (uint8_t _offset = 0);

  inline void flush (void) { TWIC.stop(); }
  inline OLED_SH1107_Class& setFontSet (const uint8_t* _ptr) { FontSet = (uint8_t*)_ptr; return *this; }
  inline OLED_SH1107_Class& setExtTableInROM (const uint8_t* _ptr) { ExtraROMSet = (uint8_t*)_ptr; return *this; }
  inline OLED_SH1107_Class& setExtTableInRAM (void* _ptr) { ExtraRAMSet = (uint8_t*)_ptr; return *this; }

  OLED_SH1107_Class& setExtFont (uint8_t* _ram_table, uint8_t _c, uint8_t _logic = OLED_SET);
  OLED_SH1107_Class& drawChar (uint8_t _ax, uint8_t _ay, uint8_t _c);
  OLED_SH1107_Class& newLine (uint8_t _ay, bool _scroll = false);

  size_t write (const uint8_t _c);
  using Print::write; // pull in write(str) and write(buf, size) from Print

  inline OLED_SH1107_Class& send (const uint8_t _c) { write(_c); return *this; }
  inline OLED_SH1107_Class& send (const char* _buffer, size_t _length) {
    write((const uint8_t *) _buffer, _length);
    return *this;
  }

};

// end of code
