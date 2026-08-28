#include "Logger.h"

// define the constructor
Logger::Logger() {
  _err_shout = false;
}

// define all functions
Logger& Logger::get_instance() {
  static Logger instance;
  return instance;
}

void Logger::init() {
  Serial.begin(115200);
  log_msg("Starting", true, !PLOTTER);
}

void Logger::log_num(float f, bool line, bool plot) {
  if (plot) {
    if (line) {
      Serial.println(f);
    } else {
      Serial.print(f);
    }
  }
}

void Logger::log_msg(const String& c, bool line, bool plot) {
  if (plot) {
    if (line) {
      Serial.println(c);
    } else {
      Serial.print(c);
    }
  }
}

void Logger::log_err(uint16_t status) {
  if (!_err_shout) {
    if (bitRead(status, 8)) {
      log_msg("***** Error: Watchdog Right Backwards", true, !PLOTTER);
    }
    if (bitRead(status, 9)) {
      log_msg("***** Error: Watchdog Right Forwards", true, !PLOTTER);
    }
    if (bitRead(status, 10)) {
      log_msg("***** Error: Watchdog Left Backwards", true, !PLOTTER);
    }
    if (bitRead(status, 11)) {
      log_msg("***** Error: Watchdog Left Forwards", true, !PLOTTER);
    }
    if (bitRead(status, 14)) {
      log_msg("***** Error: Raspi CAN Timeout", true, !PLOTTER);
    }
    if (bitRead(status, 15)) {
      log_msg("***** Error: HandboxNano CAN Timeout", true, !PLOTTER);
    }
    _err_shout = true;
  }
}

void Logger::reset_err_shout() {
  _err_shout = false;
}