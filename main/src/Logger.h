#pragma once


// include all necessary libraries
#include <Arduino.h>
#include "../dev/DevOptions.h"


// define the class
class Logger {
  public:
    static Logger& get_instance();
    void init();
    void log_num(float f, bool line, bool plot);
    void log_msg(char* c, bool line, bool plot);
    void log_err(uint16_t status);
    void reset_err_shout();
  private:
    Logger();
    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;
    bool _err_shout = false;
};