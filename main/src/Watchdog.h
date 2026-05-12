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
        void reset();
        void reset_count();
    private:
        uint16_t _old_pos;
        uint16_t _count;
        bool _fired;
        const int _cnt_tresh = 10;
        const int _pos_tresh = 25;
};