#pragma once

// include all libraries
#include "Watchdog.h"

// define the class
class Motor {
    public:
        Motor();
        Motor(uint16_t pin_f, uint16_t pin_b, uint16_t pin_end_f, uint16_t pin_end_b, 
            uint16_t pin_pos, uint16_t virt_endstop);
        Motor& operator=(const Motor& other);
        void move_backw();
        void move_halt();
        void move_forw();
        void update();
        void reset();
        uint16_t get_pos();
        bool get_endstop_f();
        bool get_endstop_b();
        uint16_t get_virt_endstop();
        void set_virt_endstop(uint16_t vend);
        Watchdog& get_wd_f();
        Watchdog& get_wd_b();
    private:
        bool _endstop_f;
        bool _endstop_b;
        uint16_t _virt_endstop;
        float _filt_pos;
        const float _filtercoef = 0.75;
        uint16_t _pin_f;
        uint16_t _pin_b;
        uint16_t _pin_end_f;
        uint16_t _pin_end_b;
        uint16_t _pin_pos;
        Watchdog _wd_f;
        Watchdog _wd_b;
};