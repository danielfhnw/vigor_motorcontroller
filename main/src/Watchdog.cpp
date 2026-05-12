#include "Watchdog.h"

// define the constructor
Watchdog::Watchdog() {
  _old_pos = 0;
  _count = 0;
  _fired = false;
}

// define the assignment operator
Watchdog& Watchdog::operator=(const Watchdog& other) {
  _old_pos = other._old_pos;
  _count = other._count;
  _fired = other._fired;
  return *this;
}

// define all functions
bool Watchdog::check(uint16_t pos) {
  if (_count == 0) {
    _old_pos = pos;
  }
  if (_count < _cnt_tresh) {
    ++_count;
  } else {
    if (abs(_old_pos - pos) < _pos_tresh) {
      _fired = true;
    }
    _count = 0;
  }
  return _fired;
}

bool Watchdog::get_fired() {
  return _fired;
}

void Watchdog::reset() {
  _count = 0;
  _fired = false;
}

void Watchdog::reset_count() {
  _count = 0;
}