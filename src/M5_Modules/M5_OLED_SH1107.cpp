/**
 * @file M5_OLED_SH1107.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <M5_Modules.h>
#include <api/delay_busywait.h>

const uint8_t oled_init[] PROGMEM = {
    0x00  // command stream
  , 0xAE  // Display >0=Off
  , 0xA4  // Set Entire display : >0=Off
  , 0xA6  // Set Display : >0=Normal, >1=Reverse
  , 0x20  // Set Memory addressing mode : >0=Page addressing, 1=Vertical addressing
  , 0xC8  // Set Common output scan direction : 0=normal, >8=reverse (vertical flip)
  , 0xA1  // Set Segment re-map : 0=Normal, >1=Flip

  , 0xD5  // Set Display Divide Ratio/Oscillator Frequency Mode (POR 0x10)
  , 0x10  //   Oscillator Frequency : 8, Divide Ratio : 0
  , 0xD9  // Set Pre-charge Period Mode (POR 0x22)
  , 0x10  //
  , 0xDB  // Set VCOM Deselect Level Mode (POR 0x15)
  , 0x15  //

  , 0xA8  // Set multiplex ration
  , OLED_H * OLED_L - 1
  , 0xD3  // Set display offset : top column
  , OLED_O
  , 0xDC  // Set display start line : offset column
  , 0
};

OLED_SH1107_Class& OLED_SH1107_Class::disable (void) {
  TWIC
    .start(OLED_SH1107_ADDR, 2)
    .send(0x00)
    .send(0xAE)
  ;
  return *this;
}

void OLED_SH1107_Class::set_page (uint8_t _page, uint8_t _len) {
  TWIC
    .start(OLED_SH1107_ADDR)
    .send(0x00)
    .send(0x00)         /* Set Column Low */
    .send(0x10)         /* Set Column High */
    .send(0xB0 + _page) /* Set Page */
    .start(OLED_SH1107_ADDR, ++_len)
    .send(0x40)         /* RAM Write Mode */
  ;
}

OLED_SH1107_Class& OLED_SH1107_Class::clear (bool console_mode) {
  TWIC
    .start(OLED_SH1107_ADDR)
    .write(P(oled_init), sizeof(oled_init))
  ;
  uint8_t _x = OLED_W;
  do {
    uint8_t _y = OLED_H * OLED_L;
    set_page(--_x, _y);
    do { TWIC.write(0x00); } while (--_y);
  } while (_x);
  _console_mode = console_mode;
  _cx = _cy = 0;
  _cz = OLED_V;
  return setContrast(_ct);
}

OLED_SH1107_Class& OLED_SH1107_Class::setFlip (bool flip_mode) {
  TWIC
    .start(OLED_SH1107_ADDR, 3)
    .send(0x00)
    .send(flip_mode ? 0xC0 : 0xC8)
    .send(flip_mode ? 0xA0 : 0xA1)
  ;
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::setRevesible (bool reverse_mode) {
  TWIC
    .start(OLED_SH1107_ADDR, 2)
    .send(0x00)
    .send(reverse_mode ? 0xA7 : 0xA6)
  ;
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::setContrast (uint8_t _contrast) {
  _ct = _contrast;
  TWIC
    .start(OLED_SH1107_ADDR, 4)
    .send(0x00)
    .send(0x81)
    .send(_contrast)
    .send(0xAF)     // Display >1=On
  ;
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::setScroll (uint8_t _offset) {
  TWIC
    .start(OLED_SH1107_ADDR, 3)
    .send(0x00)
    .send(0xDC)
    .send(_offset & (OLED_W * OLED_H - 1))
  ;
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::setPosition (uint8_t _x, uint8_t _y) {
  _cx = _x;
  if (_console_mode) _cy = (_cz + _y) & (OLED_H - 1);
  else _cy = _y;
  return *this;
}

const uint8_t test_data[] PROGMEM = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

OLED_SH1107_Class& OLED_SH1107_Class::drawTestPattern (uint8_t _offset) {
  uint8_t _x = OLED_W;
  do {
    uint8_t _y = OLED_W * OLED_H;
    set_page(--_x, _y);
    do {
      TWIC.write(pgm_read_byte(&test_data[(_offset + --_y) & 7]));
    } while (_y);
  } while (_x);
  TWIC.stop();
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::drawChar (uint8_t _ax, uint8_t _ay, uint8_t _c) {
  uint8_t* _p;
  _c -= 32;
  if (_c >= 240) _p = ExtraRAMSet + ((uint16_t)(_c - 240) << 4);
  else if (_c >= 96 && ExtraROMSet != nullptr) _p = ExtraROMSet + ((uint16_t)(_c - 96) << 4);
  else _p = (uint8_t*)&FontSet[(uint16_t)_c << 4];
  uint8_t _i = OLED_L;
  TWIC
    .start(OLED_SH1107_ADDR)
    .send(0x00)
    .send((uint8_t)(0xB0 + (~_ax & (OLED_W - 1))))
    .send(0x00)
    .send((uint8_t)(0x10 + (_ay & (OLED_H - 1))))
    .start(OLED_SH1107_ADDR, _i + 1)
    .send(0x40)
  ;
  if (_c >= 240) {
    do TWIC.write(*_p++); while (--_i);
  }
  else {
    do TWIC.write(pgm_read_byte(_p++)); while (--_i);
  }
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::newLine (uint8_t _ay, bool _scroll) {
  uint8_t _d = OLED_L;
  TWIC
    .start(OLED_SH1107_ADDR)
    .send(0x00)
    .send(0x21) /* Virtical Line Mode */
    .send(0xB0) /* Set Page */
  ;
  do {
    TWIC
      .start(OLED_SH1107_ADDR)
      .send(0x00)
      .send(0x10 - _d) /* Set Column */
      .send(0x10 + (_ay & (OLED_H - 1)) )
      .start(OLED_SH1107_ADDR)
      .send(0x40)
    ;
    uint8_t _c = OLED_W;
    do TWIC.write(0x00); while (--_c);
    if (_scroll) setScroll(((_ay + OLED_V + 1) << 4) - _d + 1);
  } while (--_d);
  TWIC
    .start(OLED_SH1107_ADDR, 2)
    .send(0x00)
    .send(0x20) /* Normal Line Mode */
  ;
  return *this;
}

OLED_SH1107_Class& OLED_SH1107_Class::setExtFont (uint8_t* _ram_table, uint8_t _c, uint8_t _logic) {
  uint8_t *_p = (uint8_t*)&FontSet[(uint16_t)(_c - 32) << 4];
  uint8_t _i = OLED_L;
  do {
    uint8_t _d = (_c < 32) ? 0xff : pgm_read_byte(_p++);
    uint8_t _e = *_ram_table;
    if (_logic == OLED_XOR) _e ^= _d;
    else if (_logic == OLED_AND) _e &= _d;
    else if (_logic == OLED_OR) _e |= _d;
    else _e = _d;
    *(_ram_table++) = _e;
  } while (--_i);
  return *this;
}

size_t OLED_SH1107_Class::write (const uint8_t _c) {
  if (_c == '\b') { _cx -= _cx ? 1 : 0; }
  else if (_c == '\r') { _cx = 0; }
  else if (_c == '\f') { clear(_console_mode); }
  else if (_c == '\n') {
    if (++_cy != _cz) { newLine(_cy, false); }
    else { _cz++; newLine(_cy, _console_mode); }
  }
  else {
    if (_cx >= OLED_W) {
      _cx = 0;
      if (++_cy != _cz) { newLine(_cy, false); }
      else { _cz++; newLine(_cy, _console_mode); }
    }
    if (_c == '\t') _cx = (_cx & ~3) + 4;
    else drawChar(_cx++, _cy, _c);
  }
  return 1;
}

// end of code
