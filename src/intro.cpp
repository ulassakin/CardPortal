#include <M5Cardputer.h>
#include <M5GFX.h>
#include "fonts/PressStart2P_Regular7pt7b.h"   // My font
#include "intro.h"

#define COL_BG     BLACK
#define COL_PURPLE 0x780F
#define COL_PINK   0xF81F
#define COL_BLUE   0x07FF
#define COL_WHITE  WHITE

M5GFX &dsp = M5Cardputer.Display;

inline int rnd(int a, int b) { return random(a, b); }

// Basic scanline effect
void drawScanlines() {
  for (int i = 0; i < 3; i++) {
    int y = rnd(20, dsp.height()-20);
    dsp.drawFastHLine(0, y, dsp.width(), (random(2) ? COL_PINK : COL_BLUE));
  }
}

// Neon card + portal logo
void drawCardPortalLogo(int cx, int cy) {
  dsp.drawRoundRect(cx-18, cy-12, 36, 24, 4, COL_PURPLE);
  dsp.drawRoundRect(cx-19, cy-13, 38, 26, 5, COL_PINK);
  dsp.drawCircle(cx, cy, 6, COL_PURPLE);
  dsp.drawCircle(cx, cy, 3, COL_PINK);
}

// Draw title with a glitch effect
void drawGlitchTitle(const char* txt, int baseX, int baseY, uint16_t col) {
  dsp.setFont(&PressStart2P_Regular7pt7b);
  dsp.setTextColor(col);

  dsp.setCursor(baseX + rnd(-1,2), baseY + rnd(-1,2));
  dsp.print(txt);

  dsp.setTextColor(COL_PURPLE);
  dsp.setCursor(baseX, baseY);
  dsp.print(txt);

  int textW = dsp.textWidth(txt);
  int logoX = baseX + textW + 20;
  int logoY = baseY - dsp.fontHeight()/2;
  drawCardPortalLogo(logoX, logoY);
}

// My intro function to run it on main.cpp
void runMinimalIntro() {
  const char* title = "CARDPORTAL";

  for (int frame = 0; frame < 12; frame++) {
    dsp.fillScreen(COL_BG);
    drawGlitchTitle(title, 30, 60, (frame % 2 ? COL_PINK : COL_BLUE));
    drawScanlines();
    delay(120);
  }

  dsp.fillScreen(COL_BG);
  dsp.setFont(&PressStart2P_Regular7pt7b);
  dsp.setTextColor(COL_PURPLE);
  dsp.setCursor(30, 60);
  dsp.print(title);

  int textW = dsp.textWidth(title);
  int logoX = 30 + textW + 20;
  int logoY = 60 - dsp.fontHeight()/2;
  drawCardPortalLogo(logoX, logoY);

  delay(2000);
}
