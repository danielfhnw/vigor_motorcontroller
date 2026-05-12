#pragma once

// include all necessary libraries
#include <EEPROM.h>
#include "Logger.h"

// define the class
class Memory {
    public:
        static Memory& get_instance();
        uint16_t read_counter();
        uint16_t get_offset();
        uint16_t read_vend_l();
        uint16_t read_vend_r();
        bool write_vend(uint16_t vend_l, uint16_t vend_r);
        uint16_t write_vend_sw(uint16_t vend_l, uint16_t vend_r);
        void reset_saveflag();
        uint16_t validate_2oo3(uint16_t a, uint16_t b, uint16_t c);
    private:
        Memory();
        Memory(Memory const&) = delete;
        Memory& operator=(Memory const&) = delete;
        const int _dt_save = 1000;
        bool _save_flag;
        unsigned long _old_save_time;
        uint16_t _offset;
};