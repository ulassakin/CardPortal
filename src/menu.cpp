#include "M5Cardputer.h"
#include "M5GFX.h"
#define COL_BG     BLACK
#define COL_PURPLE 0x780F
#define COL_PINK   0xF81F
#define COL_BLUE   0x07FF
#define COL_WHITE  WHITE
#include "fonts/PressStart2P_Regular7pt7b.h"

enum AppState {
    PRESS,
    ENTER_SSID,
    ENTER_PASS,
    SELECT_HTML,
    RUNNING,
};

String SSID = "";
String pass = "";
int cursorPosition = 0;

bool cursorVisible = true;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 500; // ms

M5GFX &dsp = M5Cardputer.Display;
bool pressDrawn = false;
AppState currentState = PRESS;

M5Canvas portalCanvas(&M5Cardputer.Display);

void drawPress() {
    dsp.fillScreen(COL_BG);
    dsp.setFont(&PressStart2P_Regular7pt7b);
    dsp.setTextColor(COL_PURPLE);
    dsp.setCursor(30, 60);
    dsp.print("CardPortal");

    int textW = dsp.textWidth("CardPortal");
    int logoX = 30 + textW + 20;
    int logoY = 60 - dsp.fontHeight()/2;
    dsp.drawRoundRect(logoX-18, logoY-12, 36, 24, 4, COL_PURPLE);
    dsp.drawRoundRect(logoX-19, logoY-13, 38, 26, 5, COL_PINK);
    dsp.drawCircle(logoX, logoY, 6, COL_PURPLE);
    dsp.drawCircle(logoX, logoY, 3, COL_PINK);

    dsp.setCursor(20,90);
    dsp.print("Press To Start");
    
}

void menu_loop() {
    M5Cardputer.update();

    switch (currentState) {
        case PRESS: {
            if (!pressDrawn) {   // sadece ilk kez girince çiz
                drawPress();
                pressDrawn = true;
            }
            break;
        }
    }
}