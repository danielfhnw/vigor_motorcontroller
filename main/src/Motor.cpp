#include "Motor.h"

// define the constructor
Motor::Motor() {
  _pin_f = 0;
  _pin_b = 0;
  _pin_end_f = 0;
  _pin_end_b = 0;
  _pin_pos = 0;
  _virt_endstop = 0;
  _endstop_f = false;
  _endstop_b = false;
  _filt_pos = 0;
  _wd_f = Watchdog();
  _wd_b = Watchdog();
}

Motor::Motor(uint16_t pin_f, uint16_t pin_b, uint16_t pin_end_f, uint16_t pin_end_b, 
  uint16_t pin_pos, uint16_t virt_endstop)
    : _pin_f(pin_f),
      _pin_b(pin_b),
      _pin_end_f(pin_end_f),
      _pin_end_b(pin_end_b),
      _pin_pos(pin_pos),
      _virt_endstop(virt_endstop)
{
    _endstop_f = digitalRead(pin_end_f);
    _endstop_b = digitalRead(pin_end_b);
    _filt_pos = analogRead(pin_pos);
}

// define the assignment operator
Motor& Motor::operator=(const Motor& other) {
  _pin_f = other._pin_f;
  _pin_b = other._pin_b;
  _pin_end_f = other._pin_end_f;
  _pin_end_b = other._pin_end_b;
  _pin_pos = other._pin_pos;
  _virt_endstop = other._virt_endstop;
  _endstop_f = other._endstop_f;
  _endstop_b = other._endstop_b;
  _filt_pos = other._filt_pos;
  _wd_f = other._wd_f;
  _wd_b = other._wd_b;
  return *this;
}

// define all functions
void Motor::move_backw() {
  if (!_wd_b.get_fired() && !_endstop_b) {
    digitalWrite(_pin_f, LOW);
    digitalWrite(_pin_b, HIGH);
    _wd_f.reset_count();
    _wd_b.check(_filt_pos);
  } else {
    move_halt();
  }
}

void Motor::move_halt() {
  digitalWrite(_pin_f, LOW);
  digitalWrite(_pin_b, LOW);
  _wd_f.reset_count();
  _wd_b.reset_count();
}

void Motor::move_forw() {
  if (!_wd_f.get_fired() && !_endstop_f) {
    digitalWrite(_pin_f, HIGH);
    digitalWrite(_pin_b, LOW);
    _wd_b.reset_count();
    _wd_f.check(_filt_pos);
  } else {
    move_halt();
  }
}

void Motor::update() {
  uint16_t temp = analogRead(_pin_pos);
  _filt_pos = _filtercoef*_filt_pos + (1-_filtercoef)*temp;
  _endstop_f = digitalRead(_pin_end_f);
  _endstop_b = digitalRead(_pin_end_b);
}

void Motor::reset() {
  _wd_f.reset();
  _wd_b.reset();
}

uint16_t Motor::get_pos() {
  return _filt_pos;
}

bool Motor::get_endstop_f() {
  return _endstop_f;
}

bool Motor::get_endstop_b() {
  return _endstop_b;
}

uint16_t Motor::get_virt_endstop() {
  return _virt_endstop;
}

void Motor::set_virt_endstop(uint16_t vend) {
  _virt_endstop = vend;
}

Watchdog& Motor::get_wd_f() {
  return _wd_f;
}

Watchdog& Motor::get_wd_b() {
  return _wd_b;
}