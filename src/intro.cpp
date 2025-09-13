#include <M5Cardputer.h>
#include <M5GFX.h>
#include "fonts/PressStart2P_Regular7pt7b.h"   // My font
#include "intro.h"

#define COL_BG     BLACK
#define COL_PURPLE 0x780F
#define COL_PINK   0xF81F
#define COL_BLUE   0x07FF
#define COL_WHITE  WHITE


inline int rnd(int a, int b) { return random(a, b); }

// Basic scanline effect
void drawScanlines() {
  for (int i = 0; i < 3; i++) {
    int y = rnd(20, M5Cardputer.Display.height()-20);
    M5Cardputer.Display.drawFastHLine(0, y, M5Cardputer.Display.width(), (random(2) ? COL_PINK : COL_BLUE));
  }
}

// Neon card + portal logo
void drawCardPortalLogo(int cx, int cy) {
  M5Cardputer.Display.drawRoundRect(cx-18, cy-12, 36, 24, 4, COL_PURPLE);
  M5Cardputer.Display.drawRoundRect(cx-19, cy-13, 38, 26, 5, COL_PINK);
  M5Cardputer.Display.drawCircle(cx, cy, 6, COL_PURPLE);
  M5Cardputer.Display.drawCircle(cx, cy, 3, COL_PINK);
}

// Draw title with a glitch effect
void drawGlitchTitle(const char* txt, int baseX, int baseY, uint16_t col) {
  M5Cardputer.Display.setFont(&PressStart2P_Regular7pt7b);
  M5Cardputer.Display.setTextColor(col);

  M5Cardputer.Display.setCursor(baseX + rnd(-1,2), baseY + rnd(-1,2));
  M5Cardputer.Display.print(txt);

  M5Cardputer.Display.setTextColor(COL_PURPLE);
  M5Cardputer.Display.setCursor(baseX, baseY);
  M5Cardputer.Display.print(txt);

  int textW = M5Cardputer.Display.textWidth(txt);
  int logoX = baseX + textW + 20;
  int logoY = baseY - M5Cardputer.Display.fontHeight()/2;
  drawCardPortalLogo(logoX, logoY);
}

void drawRandomChars(int count) {
  for (int i = 0; i < count; i++) {
    int x = rnd(0, M5Cardputer.Display.width() - 10);
    int y = rnd(0, M5Cardputer.Display.height() - 10);

    char c = (char)rnd(33, 126); // printable ASCII
    uint16_t col = (random(2) ? COL_PINK : COL_BLUE);

    M5Cardputer.Display.setTextColor(col);
    M5Cardputer.Display.setCursor(x, y);
    M5Cardputer.Display.print(c);
  }
}

// My intro function to run it on main.cpp
void runMinimalIntro() {
  const char* title = "CardPortal";

  for (int frame = 0; frame < 12; frame++) {
    M5Cardputer.Display.fillScreen(COL_BG);

    drawGlitchTitle(title, 30, 60, (frame % 2 ? COL_PINK : COL_BLUE));

    drawScanlines();

    drawRandomChars(10 + rnd(0, 10)); // 10-20 karakter arası

    delay(120);
  }

  M5Cardputer.Display.fillScreen(COL_BG);
  M5Cardputer.Display.setFont(&PressStart2P_Regular7pt7b);
  M5Cardputer.Display.setTextColor(COL_PURPLE);
  M5Cardputer.Display.setCursor(30, 60);
  M5Cardputer.Display.print(title);

  int textW = M5Cardputer.Display.textWidth(title);
  int logoX = 30 + textW + 20;
  int logoY = 60 - M5Cardputer.Display.fontHeight()/2;
  drawCardPortalLogo(logoX, logoY);

  delay(2000);
}

