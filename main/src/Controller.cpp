#include "Controller.h"

// define the constructor
Controller::Controller() {
  _motor = Motor();
  _pin_but_f = 0;
  _pin_but_b = 0;
  _inverted = false;
  _ref = 0;
  _old_time = 0;
}

Controller::Controller(uint16_t pin_f, uint16_t pin_b, uint16_t pin_end_f, uint16_t pin_end_b,
  uint16_t pin_pos, uint16_t pin_but_f, uint16_t pin_but_b, bool inverted, uint16_t virt_entstop) :
    _motor(pin_f, pin_b, pin_end_f, pin_end_b, pin_pos, virt_entstop),
    _pin_but_f(pin_but_f),
    _pin_but_b(pin_but_b),
    _inverted(inverted)
{
  _old_time = millis();
  _ref = 0;
}

// define the assignment operator
Controller& Controller::operator=(const Controller& other) {
  _motor = other._motor;
  _pin_but_f = other._pin_but_f;
  _pin_but_b = other._pin_but_b;
  _inverted = other._inverted;
  _ref = other._ref;
  _old_time = other._old_time;
  return *this;
}

// define all functions
void Controller:: update() {
  _motor.update();
}

void Controller::update_ref(uint16_t ref) {
  if (_inverted) {
    _ref = (100 - float(ref)) * (923 - _motor.get_virt_endstop())/100 + _motor.get_virt_endstop();
  } else {
    _ref = float(ref) * (_motor.get_virt_endstop() - 100)/100 + 100;
  }
}

void Controller::move_auto() {
  if (millis() - _old_time > _dt) {
    _old_time = millis();

    if (abs(_ref - _motor.get_pos()) > _tresh) {
      if (_ref - _motor.get_pos() < 0) {
        _motor.move_backw();
      } else {
        _motor.move_forw();
      }
    } else {
      _motor.move_halt();
    }
  }
}

void Controller::move_semi() {
  if (digitalRead(_pin_but_f)) {
      _ref = _motor.get_virt_endstop();
    }
  if (_inverted) {
    if (digitalRead(_pin_but_b)) {
      _ref = 1023;
    }
  } else {
    if (digitalRead(_pin_but_b)) {
      _ref = 0;
    }
  }
  move_auto();
}

void Controller::move_home() {
  if (millis() - _old_time > _dt) {
    _old_time = millis();

    if (_inverted){
      if (!_motor.get_endstop_f()) {
        _motor.move_forw();
      } else {
        _motor.move_halt();
      }
    } else {
      if (!_motor.get_endstop_b()) {
        _motor.move_backw();
      } else {
        _motor.move_halt();
      }
    }
  }
}

void Controller::move_halt() {
  _motor.move_halt();
}

Motor& Controller::get_motor() {
  return _motor;
}

bool Controller::get_inverted() {
  return _inverted;
}
