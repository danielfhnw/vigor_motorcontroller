// version 5.0 19.08.2025

// import custom settings
#include "dev/DevOptions.h"

// import custom libraries
#include "src/PinMap.h"
#include "src/Memory.h"
#include "src/Logger.h"
#include "src/CANClass.h"
#include "src/Controller.h"


// define all constants
#define STATE_SEMI 1
#define STATE_AUTO 2
#define STATE_MANUAL 3
#define STATE_ERR 4
const int dt_loop = 2;

// define all variables
unsigned long old_loop_time = 0;
uint16_t curr_state = STATE_MANUAL;
Controller controller_l;
Controller controller_r;


void setup() {
  Pinmap::init();
  Logger::get_instance().init();
  CANClass::get_instance().init(); 
  CANClass::get_instance().set_reg(REG_L_VEND, Memory::get_instance().read_vend_l());
  CANClass::get_instance().set_reg(REG_R_VEND, Memory::get_instance().read_vend_r());
  CANClass::get_instance().set_reg(REG_MEM_CNT, Memory::get_instance().read_counter());
  CANClass::get_instance().set_reg(REG_MEM_OFF, Memory::get_instance().get_offset());
  
  controller_l = Controller(MOT_L_FORW, MOT_L_BACKW, END_L_FORW, END_L_BACKW, 
    POS_L, BUT_OPEN, BUT_CLOSE, Pinmap::read_dip_switch(DIP_INVERTED_L), CANClass::get_instance().get_reg(REG_L_VEND));
  controller_r = Controller(MOT_R_FORW, MOT_R_BACKW, END_R_FORW, END_R_BACKW, 
    POS_R, BUT_OPEN, BUT_CLOSE, Pinmap::read_dip_switch(DIP_INVERTED_R), CANClass::get_instance().get_reg(REG_R_VEND));
  
  Logger::get_instance().log_msg("Starting Vigor Motorcontroller", true, !PLOTTER);
  Logger::get_instance().log_msg("Inverted left: " + String(Pinmap::read_dip_switch(DIP_INVERTED_L)), true, !PLOTTER);
  Logger::get_instance().log_msg("Inverted right: " + String(Pinmap::read_dip_switch(DIP_INVERTED_R)), true, !PLOTTER);

  if (Pinmap::read_dip_switch(DIP_DISABLE_WD_L_F)) controller_l.get_motor().get_wd_f().disable();
  if (Pinmap::read_dip_switch(DIP_DISABLE_WD_R_F)) controller_r.get_motor().get_wd_f().disable();
  if (Pinmap::read_dip_switch(DIP_DISABLE_WD_L_B)) controller_l.get_motor().get_wd_b().disable();
  if (Pinmap::read_dip_switch(DIP_DISABLE_WD_R_B)) controller_r.get_motor().get_wd_b().disable();

  Logger::get_instance().log_msg("Left watchdog forward: " + String(!controller_l.get_motor().get_wd_f().get_disabled()), true, !PLOTTER);
  Logger::get_instance().log_msg("Left watchdog backward: " + String(!controller_l.get_motor().get_wd_b().get_disabled()), true, !PLOTTER);
  Logger::get_instance().log_msg("Right watchdog forward: " + String(!controller_r.get_motor().get_wd_f().get_disabled()), true, !PLOTTER);
  Logger::get_instance().log_msg("Right watchdog backward: " + String(!controller_r.get_motor().get_wd_b().get_disabled()), true, !PLOTTER);

  Logger::get_instance().log_msg("Starting in Manual mode", true, !PLOTTER);
}

void loop() {
  if (millis() - old_loop_time > dt_loop) {
    old_loop_time = millis();

    // update controllers
    controller_l.update();
    controller_r.update();

    // update CAN Registers
    CANClass::get_instance().update(update_status(), controller_l.get_motor().get_pos(), 
      controller_r.get_motor().get_pos(), controller_l.get_motor().get_virt_endstop(), 
      controller_r.get_motor().get_virt_endstop(), Memory::get_instance().read_counter(), 
      Memory::get_instance().get_offset());

    
    switch (curr_state) {
      case STATE_SEMI: // Semiautomatik
        // Fahrbefehl
        controller_l.move_semi();
        controller_r.move_semi();

        // Stateupdate
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_AUTO)){
          update_state(STATE_AUTO);
        } 
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_MANUAL)){
          update_state(STATE_MANUAL);
        }
        break;


      case STATE_AUTO: // Automatic mode
        // update all controllers according to the reference value
        controller_l.update_ref(CANClass::get_instance().get_reg(REG_L_REF));
        controller_r.update_ref(CANClass::get_instance().get_reg(REG_R_REF));

        // Fahrbefehl
        controller_l.move_auto();
        controller_r.move_auto();
        
        // Stateupdate
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_SEMI)){
          update_state(STATE_SEMI);
        }
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_MANUAL)){
          update_state(STATE_MANUAL);
        } 
        break;


      case STATE_MANUAL: // Manual mode
        // Fahrbefehl
        controller_l.move_halt();
        controller_r.move_halt();

        // Speichern der virtuellen Endanschläge
       

        // Stateupdate
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_SEMI)){
          update_state(STATE_SEMI);
        }
        if (bitRead(CANClass::get_instance().get_reg(REG_HB_IN), BIT_AUTO)){
          update_state(STATE_AUTO);
        }
        break;


      case STATE_ERR: // Error
        // Fahrbefehl
        controller_l.move_home();
        controller_r.move_home();

        // Fehlerausgabe
        Logger::get_instance().log_err(CANClass::get_instance().get_reg(REG_STATUS));
        break;
    }

  }

  // read incoming CAN messages
  CANClass::get_instance().read_and_digest();

}

void update_state(uint16_t state) {
  if (curr_state == state) return;    // do nothing if state is already set
  curr_state = state;
  controller_l.move_halt();
  controller_r.move_halt();
  CANClass::get_instance().set_reg(REG_L_REF, 0);
  CANClass::get_instance().set_reg(REG_R_REF, 0);
  controller_l.update_ref(CANClass::get_instance().get_reg(REG_L_REF));
  controller_r.update_ref(CANClass::get_instance().get_reg(REG_R_REF));
  switch(state) {
    case STATE_SEMI:
      Logger::get_instance().log_msg("Switching to Semiautomatic mode", true, !PLOTTER);
      break;
    case STATE_AUTO:
      Logger::get_instance().log_msg("Switching to Automatic mode", true, !PLOTTER);
      break;
    case STATE_MANUAL:
      Logger::get_instance().log_msg("Switching to Manual mode", true, !PLOTTER);
      break;
    case STATE_ERR:
      Logger::get_instance().log_msg("Switching to Error state", true, !PLOTTER);
      break;
  }
}

uint16_t update_status(){
  uint16_t status = CANClass::get_instance().get_reg(REG_STATUS);

  // check for commands
  uint16_t cmd = CANClass::get_instance().get_reg(REG_CMD);
  if (bitRead(cmd, BIT_SAVE_VEND)){             // save new vend from CAN
    uint16_t vend_l = CANClass::get_instance().get_reg(REG_L_VEND);
    controller_l.get_motor().set_virt_endstop(vend_l);
    uint16_t vend_r = CANClass::get_instance().get_reg(REG_R_VEND);
    controller_r.get_motor().set_virt_endstop(vend_r);
    Memory::get_instance().write_vend_sw(vend_l, vend_r);
    Logger::get_instance().log_msg("saved new virtual endstops", true, !PLOTTER);
  }
  if (bitRead(cmd, BIT_RESET_ERR)){             // reset all errors
    controller_l.get_motor().reset();
    controller_r.get_motor().reset();
    status = bitClear(status, BIT_R_F_WD);
    status = bitClear(status, BIT_R_B_WD);
    status = bitClear(status, BIT_L_F_WD);
    status = bitClear(status, BIT_L_B_WD);
    CANClass::get_instance().set_reg(REG_HEARTBEAT_RPI, HEARTBEAT_INI);
    status = bitClear(status, BIT_HB_RPI);
    CANClass::get_instance().set_reg(REG_HEARTBEAT_HBN, HEARTBEAT_INI);
    status = bitClear(status, BIT_HB_HBN);
    Logger::get_instance().log_msg("reset all errors", true, !PLOTTER);
  }
  if (bitRead(cmd, BIT_JUMP_ERR)){             // jump out of error state
    update_state(STATE_MANUAL);
    Logger::get_instance().reset_err_shout();
    Logger::get_instance().log_msg("jumped out of error state", true, !PLOTTER);
  }
  if (cmd){
    CANClass::get_instance().set_reg(REG_CMD, 0);       // reset command register
  }

  status = bitWrite(status, BIT_L_F_END, controller_l.get_motor().get_endstop_f());
  status = bitWrite(status, BIT_L_B_END, controller_l.get_motor().get_endstop_b());
  status = bitWrite(status, BIT_R_F_END, controller_r.get_motor().get_endstop_f());
  status = bitWrite(status, BIT_R_B_END, controller_r.get_motor().get_endstop_b());

  status = bitWrite(status, 4, bitRead(curr_state, 0));
  status = bitWrite(status, 5, bitRead(curr_state, 1));
  status = bitWrite(status, 6, bitRead(curr_state, 2));

  status = bitWrite(status, BIT_R_F_WD, controller_r.get_motor().get_wd_f().get_fired());
  status = bitWrite(status, BIT_R_B_WD, controller_r.get_motor().get_wd_b().get_fired());
  status = bitWrite(status, BIT_L_F_WD, controller_l.get_motor().get_wd_f().get_fired());
  status = bitWrite(status, BIT_L_B_WD, controller_l.get_motor().get_wd_b().get_fired());

  if (bitRead(status, BIT_R_F_WD) || bitRead(status, BIT_R_B_WD) || bitRead(status, BIT_L_F_WD) || bitRead(status, BIT_L_B_WD)){
    update_state(STATE_ERR);
  }

  status = bitWrite(status, BIT_L_INV, controller_l.get_inverted());
  status = bitWrite(status, BIT_R_INV, controller_r.get_inverted());

  if (curr_state == 2){   // check raspi connection only in automatic mode
    status = bitWrite(status, BIT_HB_RPI, CANClass::get_instance().reduce_heartbeat_rpi());
    if (bitRead(status, BIT_HB_RPI)){
      update_state(STATE_ERR);
    } else {
      if (curr_state == STATE_ERR && !(bitRead(status, BIT_R_F_WD) || bitRead(status, BIT_R_B_WD) || bitRead(status, BIT_L_F_WD) || bitRead(status, BIT_L_B_WD))) {
        update_state(STATE_AUTO);
        Logger::get_instance().reset_err_shout();
        Logger::get_instance().log_msg("jumped out of error state due to hb raspi reconnection", true, !PLOTTER);
      }
    }
  }
  status = bitWrite(status, BIT_HB_HBN, CANClass::get_instance().reduce_heartbeat_hbn());
  if (bitRead(status, BIT_HB_HBN)){
      update_state(STATE_ERR);
  } else {
    if (curr_state == STATE_ERR && !(bitRead(status, BIT_R_F_WD) || bitRead(status, BIT_R_B_WD) || bitRead(status, BIT_L_F_WD) || bitRead(status, BIT_L_B_WD))) {
      update_state(STATE_AUTO);
      Logger::get_instance().reset_err_shout();
      Logger::get_instance().log_msg("jumped out of error state due to hb nano reconnection", true, !PLOTTER);
    }
  }
 
  return status;
}