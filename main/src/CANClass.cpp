#include "CANClass.h"

// define the constructor
CANClass::CANClass() : _sender(10) {
    _frame.can_id = 0;
    _frame.can_dlc = FRAME_SIZE;
    _frame.data[0] = 0;
    _frame.data[1] = 0;
    _heartbeat_rpi = HEARTBEAT_INI;
    _heartbeat_hbn = HEARTBEAT_INI;
    _status = 0;
    _ref_left = 0;
    _pos_left = 0;
    _geo_left = 0;
    _vend_left = 0;
    _ref_right = 0;
    _pos_right = 0;
    _geo_right = 0;
    _vend_right = 0;
    _hb_in = 0;
    _hb_out = 0;
    _mem_cnt = 0;
    _mem_off = 0;
    _cmd = 0;
}

// define all functions
CANClass& CANClass::get_instance() {
    static CANClass instance;
    return instance;
}

void CANClass::init() {
    _sender.reset();
    _sender.setBitrate(CAN_125KBPS, MCP_8MHZ);
    _sender.setNormalMode();
}

void CANClass::update(uint16_t status, uint16_t pos_l, uint16_t pos_r, uint16_t vend_l, uint16_t vend_r, uint16_t mem_cnt, uint16_t mem_off) {
    set_reg(REG_STATUS, status);
    set_reg(REG_L_MEAS, pos_l);
    set_reg(REG_R_MEAS, pos_r);
    // set_reg(REG_L_VEND, vend_l);
    // set_reg(REG_R_VEND, vend_r);
    set_reg(REG_MEM_CNT, mem_cnt);
    set_reg(REG_MEM_OFF, mem_off);    
}

void CANClass::set_reg(uint16_t reg, uint16_t data) {
    switch(reg) {
        case REG_HEARTBEAT_RPI:
            _heartbeat_rpi = data;
            break;
        case REG_HEARTBEAT_HBN:
            _heartbeat_hbn = data;
            break;
        case REG_STATUS:
            _status = data;
            break;
        case REG_L_REF:
            _ref_left = data;
            break;
        case REG_L_MEAS:
            _pos_left = data;
            break;
        case REG_L_VEND:
            _vend_left = data;
            break;
        case REG_L_GEO:
            _geo_left = data;
            break;
        case REG_R_REF:
            _ref_right = data;
            break;
        case REG_R_MEAS:
            _pos_right = data;
            break;
        case REG_R_VEND:
            _vend_right = data;
            break;
        case REG_R_GEO:
            _geo_right = data;
            break;
        case REG_HB_IN:
            _hb_in = data;
            break;
        case REG_HB_OUT:
            _hb_out = data;
            break;
        case REG_MEM_CNT:
            _mem_cnt = data;
            break;
        case REG_MEM_OFF:
            _mem_off = data;
            break;
        case REG_CMD:
            _cmd = data;
            break;
    }
}

uint16_t CANClass::get_reg(uint16_t reg) {
    switch(reg) {
        case REG_HEARTBEAT_RPI:
            return _heartbeat_rpi;
        case REG_HEARTBEAT_HBN:
            return _heartbeat_hbn;
        case REG_STATUS:
            return _status;
        case REG_L_REF:
            return _ref_left;
        case REG_L_MEAS:
            return _pos_left;
        case REG_L_VEND:
            return _vend_left;
        case REG_L_GEO:
            return _geo_left;
        case REG_R_REF:
            return _ref_right;
        case REG_R_MEAS:
            return _pos_right;
        case REG_R_VEND:
            return _vend_right;
        case REG_R_GEO:
            return _geo_right;
        case REG_HB_IN:
            return _hb_in;
        case REG_HB_OUT:
            return _hb_out;
        case REG_MEM_CNT:
            return _mem_cnt;
        case REG_MEM_OFF:
            return _mem_off;
        case REG_CMD:
            return _cmd;
    }
    return 0;
}

void CANClass::read_and_digest() {
    if (_sender.readMessage(&_frame) == MCP2515::ERROR_OK) {
        if (_frame.can_id == REG_REQ) {
            uint16_t reg = _frame.data[0];
            uint16_t data = get_reg(reg);
            _send(reg, data);
        } else {
            uint16_t data = _frame.data[0] | (_frame.data[1] << 8);
            set_reg(_frame.can_id, data);
        }
    }
}

bool CANClass::reduce_heartbeat_rpi() {
    if (_heartbeat_rpi > 0) {
        _heartbeat_rpi--;
        return false;
    }
    return true;
}

bool CANClass::reduce_heartbeat_hbn() {
    if (_heartbeat_hbn > 0) {
        _heartbeat_hbn--;
        return false;
    }
    return true;
}

void CANClass::_send(uint16_t reg, uint16_t data) {
    _frame.can_id = reg;
    _frame.data[0] = data & 0xFF;
    _frame.data[1] = (data >> 8) & 0xFF;
    _sender.sendMessage(&_frame);
}