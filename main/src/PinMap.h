#pragma once

// include all necessary libraries
#include <Arduino.h>
#include "../dev/DevOptions.h"


// define all constants
// define all arduino io pins
#define MOT_L_FORW 2    // output D2
#define MOT_L_BACKW 3   // output D3
#define MOT_R_FORW 4    // output D4
#define MOT_R_BACKW 5   // output D5
#define MUX_SEL 6       // input D6
#define MUX_A0 7        // output D7
#define MUX_A1 8        // output D8
#define MUX_A2 9        // output D9
// D10 is used for SPI
// D11 is used for SPI
// D12 is used for SPI
// D13 is used for SPI
#define END_R_BACKW 14  // pin A0
#define END_R_FORW 15   // pin A1
#define END_L_BACKW 16  // pin A2
#define END_L_FORW 17   // pin A3
#define BUT_OPEN 18       // pin A4
#define BUT_CLOSE 19       // pin A5

//analog Inputs
#define POS_R 6  // input A6
#define POS_L 7  // input A7

// define the class
class Pinmap {
  public:
    static void init();
    static void selectMuxChannel(uint8_t channel);
};