#pragma once

// include all necessary libraries
#include "Motor.h"

// define the class
class Controller {
  public:
    Controller();
    Controller(uint16_t pin_f, uint16_t pin_b, uint16_t pin_end_f, uint16_t pin_end_b, 
        uint16_t pin_pos, uint16_t pin_but_f, uint16_t pin_but_b, bool inverted, uint16_t virt_entstop);
    Controller& operator=(const Controller& other);
    void update();
    void update_ref(uint16_t ref);
    void move_auto();
    void move_semi();
    void move_home();
    void move_halt();
    Motor& get_motor();
    bool get_inverted();
  private:
    Motor _motor;
    uint16_t _pin_but_f;
    uint16_t _pin_but_b;
    bool _inverted;
    float _ref;
    unsigned long _old_time;
    const int _dt = 50;
    const int _tresh = 8;
};