#pragma once
#include "M5Cardputer.h"
#include "M5GFX.h"

enum AppState {
    PRESS,
    ENTER_SSID,
    SELECT_HTML,
    RUNNING,
};

extern AppState currentState;

void menu_loop();
void drawPress();
void menu_setup();