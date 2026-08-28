#include "PinMap.h"

void Pinmap::init() {
    // define all pinmondes
    pinMode(MOT_L_FORW, OUTPUT);
    pinMode(MOT_L_BACKW, OUTPUT);
    pinMode(MOT_R_FORW, OUTPUT);
    pinMode(MOT_R_BACKW, OUTPUT);
    pinMode(MUX_SEL, INPUT);
    pinMode(MUX_A0, OUTPUT);
    pinMode(MUX_A1, OUTPUT);
    pinMode(MUX_A2, OUTPUT);
    pinMode(END_R_FORW, INPUT);
    pinMode(END_R_BACKW, INPUT);
    pinMode(END_L_FORW, INPUT);
    pinMode(END_L_BACKW, INPUT);
    pinMode(BUT_OPEN, INPUT_PULLUP);
    pinMode(BUT_CLOSE, INPUT_PULLUP);
}