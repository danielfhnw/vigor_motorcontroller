#pragma once

// include all libraries
#include <Arduino.h>

// define the class
class Watchdog {
    public:
        Watchdog();
        Watchdog& operator=(const Watchdog& other);
        bool check(uint16_t pos);
        bool get_fired();
        bool get_disabled();
        void disable() {_disabled = true;}
        void enable() {_disabled = false;}
        void reset();
        void reset_count();
    private:
        uint16_t _old_pos;
        uint16_t _count;
        bool _fired;
        bool _disabled = false;
        const int _cnt_tresh = 10;
        const int _pos_tresh = 25;
};