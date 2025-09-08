#pragma once
#include "M5Cardputer.h"
#include "M5GFX.h"

enum AppState {
    PRESS,
    ENTER_SSID,
    ENTER_PASS,
    SELECT_HTML,
    RUNNING,
};

extern AppState currentState;

void menu_loop();
void drawPress();
