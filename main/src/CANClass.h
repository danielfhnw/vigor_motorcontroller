#pragma once

// define all constants
#define FRAME_SIZE 2

#define REG_HEARTBEAT_RPI 0x01  //Heartbeat Raspberry Pi
#define REG_HEARTBEAT_HBN 0x02  //Heartbeat Handbox Nano

#define REG_STATUS 0x05   //Status register
#define BIT_L_F_END 0
#define BIT_L_B_END 1
#define BIT_R_F_END 2
#define BIT_R_B_END 3
#define BIT_R_F_WD 8
#define BIT_R_B_WD 9
#define BIT_L_F_WD 10
#define BIT_L_B_WD 11
#define BIT_L_INV 12
#define BIT_R_INV 13
#define BIT_HB_RPI 14
#define BIT_HB_HBN 15

#define REG_L_REF 0x10    //Setpoint left
#define REG_L_MEAS 0x11   //actual value left
#define REG_L_VEND 0x12   //Endpoint left
#define REG_L_GEO 0x13    //Geobased setpoint left
#define REG_R_REF 0x20    //Setpoint right
#define REG_R_MEAS 0x21   //actual value right
#define REG_R_VEND 0x22   //Endpoint right
#define REG_R_GEO 0x23    //Geobased setpoint right

#define REG_HB_IN 0x70    //Handbox input
#define BIT_MANUAL 0
#define BIT_SEMI 1
#define BIT_AUTO 2
#define BIT_F1 4
#define BIT_F2 5
#define BIT_F3 6
#define BIT_F4 7

#define REG_HB_OUT 0x75   //Handbox output
#define REG_MEM_CNT 0x80  //Memory counter
#define REG_MEM_OFF 0x81  //Memory offset

#define REG_CMD 0x90      //Command register
#define BIT_SAVE_VEND 0
#define BIT_RESET_ERR 1
#define BIT_JUMP_ERR 2

#define REG_REQ 0x99      //request register

#define HEARTBEAT_INI 1000

// include all libraries
#include "mcp2515.h"

// define the class
class CANClass {
    public:
        static CANClass& get_instance();
        void init();
        void update(uint16_t status, uint16_t pos_l, uint16_t pos_r, uint16_t vend_l, uint16_t vend_r, uint16_t mem_cnt, uint16_t mem_off);
        void set_reg(uint16_t reg, uint16_t value);
        uint16_t get_reg(uint16_t reg);
        void read_and_digest();
        bool reduce_heartbeat_rpi();
        bool reduce_heartbeat_hbn();
    private:
        MCP2515 _sender;
        struct can_frame _frame;
        CANClass();
        CANClass& operator=(const CANClass& other) = delete;
        uint16_t _heartbeat_rpi;
        uint16_t _heartbeat_hbn;
        uint16_t _status;
        uint16_t _ref_left;
        uint16_t _pos_left;
        uint16_t _geo_left;
        uint16_t _vend_left;
        uint16_t _ref_right;
        uint16_t _pos_right;
        uint16_t _geo_right;
        uint16_t _vend_right;
        uint16_t _hb_in;
        uint16_t _hb_out;
        uint16_t _mem_cnt;
        uint16_t _mem_off;
        uint16_t _cmd;
        void _send(uint16_t reg, uint16_t value);
};