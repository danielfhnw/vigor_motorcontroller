#include "Memory.h"

// define the constructor
Memory::Memory() {
  _save_flag = false;
  _old_save_time = millis();
  uint16_t counter = read_counter();
  _offset = 0;
  while (counter > 50000) {
    _offset += 3*6;
    counter = read_counter();
  }
}

// define the functions
Memory& Memory::get_instance() {
  static Memory instance;
  return instance;
}

uint16_t Memory::read_counter() {
  uint16_t a = EEPROM.read(0+_offset) << 8 | EEPROM.read(1+_offset);
  uint16_t b = EEPROM.read(0+_offset+6) << 8 | EEPROM.read(1+_offset+6);
  uint16_t c = EEPROM.read(0+_offset+12) << 8 | EEPROM.read(1+_offset+12);
  uint16_t counter = validate_2oo3(a, b, c);
  if (counter == 0xFFFF) {    // falls erstes Mal Lesen da EEPROM mit 0xFF initialisiert
    counter = 0;
  }
  return counter;
}

uint16_t Memory::validate_2oo3(uint16_t a, uint16_t b, uint16_t c) {
  if (a == b || a == c) {
    return a;
  } else if (b == c) {
    return b;
  } else {
    return 0xFFF0;    // falls fehlerhaftes EEPROM -> counter höher als Grenzwert aber nicht 0xFF 
  }
}

uint16_t Memory::get_offset() {
  return _offset;
}

uint16_t Memory::read_vend_l() {
  uint16_t a = EEPROM.read(2+_offset) << 8 | EEPROM.read(3+_offset);
  uint16_t b = EEPROM.read(2+_offset+6) << 8 | EEPROM.read(3+_offset+6);
  uint16_t c = EEPROM.read(2+_offset+12) << 8 | EEPROM.read(3+_offset+12);
  uint16_t vend_l = validate_2oo3(a, b, c);
  if (vend_l > 1023 || vend_l < 0) {    // falls erstes Mal Lesen da EEPROM mit 0xFF initialisiert
    vend_l = 1023;
  }
  Logger::get_instance().log_msg("virt Endanschlag links: ", false, PLOTTER);
  Logger::get_instance().log_num(vend_l, true, PLOTTER);
  return vend_l;
}

uint16_t Memory::read_vend_r() {
  uint16_t a = EEPROM.read(4+_offset) << 8 | EEPROM.read(5+_offset);
  uint16_t b = EEPROM.read(4+_offset+6) << 8 | EEPROM.read(5+_offset+6);
  uint16_t c = EEPROM.read(4+_offset+12) << 8 | EEPROM.read(5+_offset+12);
  uint16_t vend_r = validate_2oo3(a, b, c);
  if (vend_r > 1023 || vend_r < 0) {    // falls erstes Mal Lesen da EEPROM mit 0xFF initialisiert
    vend_r = 1023;
  }
  Logger::get_instance().log_msg("virt Endanschlag rechts: ", false, PLOTTER);
  Logger::get_instance().log_num(vend_r, true, PLOTTER);
  return vend_r;
}

void Memory::reset_saveflag() {
  _save_flag = false;
}

bool Memory::write_vend(uint16_t vend_l, uint16_t vend_r) {
  if (!_save_flag) {
    _save_flag = true;
    _old_save_time = millis();
  }
  if (_save_flag && millis() - _old_save_time > _dt_save) {
    uint16_t counter = write_vend_sw(vend_l, vend_r);
    _save_flag = false;
    Logger::get_instance().log_msg("Saved EEPROM Counter: ", false, PLOTTER);
    Logger::get_instance().log_num(counter, true, PLOTTER);
    Logger::get_instance().log_msg("Saved virt Endanschlag links: ", false, PLOTTER);
    Logger::get_instance().log_num(vend_l, true, PLOTTER);
    Logger::get_instance().log_msg("Saved virt Endanschlag rechts: ", false, PLOTTER);
    Logger::get_instance().log_num(vend_l, true, PLOTTER);
    return true;
  } else {
    return false;
  }
}

uint16_t Memory::write_vend_sw(uint16_t vend_l, uint16_t vend_r) {
  uint16_t counter = read_counter() + 1;
  for (int i = 0; i < 3; i++) {
    EEPROM.update(0+_offset+i*6, highByte(counter));
    EEPROM.update(1+_offset+i*6, lowByte(counter));
    EEPROM.update(2+_offset+i*6, highByte(vend_l));
    EEPROM.update(3+_offset+i*6, lowByte(vend_l));
    EEPROM.update(4+_offset+i*6, highByte(vend_r));
    EEPROM.update(5+_offset+i*6, lowByte(vend_r));
  }
  return counter;
}

