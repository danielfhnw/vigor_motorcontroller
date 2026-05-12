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
#if !INVERTED_L 
#define BUT_L_BACKW 8 
#define BUT_L_FORW 9
#else
#define BUT_L_BACKW 9
#define BUT_L_FORW 8
#endif
#if !INVERTED_R
#define BUT_R_BACKW 6
#define BUT_R_FORW 7
#else
#define BUT_R_BACKW 7
#define BUT_R_FORW 6
#endif
#define END_R_BACKW 14  // pin A0
#define END_R_FORW 15   // pin A1
#define END_L_BACKW 16  // pin A2
#define END_L_FORW 17   // pin A3

//analog Inputs
#define POS_R 6  // input A6
#define POS_L 7  // input A7

// define the class
class Pinmap {
  public:
    static void init();
};