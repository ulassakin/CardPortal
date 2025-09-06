#include <M5Cardputer.h>
#include <M5GFX.h>
#include "fonts/PressStart2P_Regular7pt7b.h"   // dönüştürdüğün retro font

#define COL_BG     BLACK
#define COL_PURPLE 0x780F
#define COL_PINK   0xF81F
#define COL_BLUE   0x07FF
#define COL_WHITE  WHITE

M5GFX &dsp = M5Cardputer.Display;

inline int rnd(int a, int b) { return random(a, b); }

// Basit scanline efekti
void drawScanlines() {
  for (int i = 0; i < 3; i++) {
    int y = rnd(20, dsp.height()-20);
    dsp.drawFastHLine(0, y, dsp.width(), (random(2) ? COL_PINK : COL_BLUE));
  }
}

// Neon kart + portal logosu
void drawCardPortalLogo(int cx, int cy) {
  // Kart çerçevesi
  dsp.drawRoundRect(cx-18, cy-12, 36, 24, 4, COL_PURPLE);
  dsp.drawRoundRect(cx-19, cy-13, 38, 26, 5, COL_PINK);

  // İçindeki portal (çift halka)
  dsp.drawCircle(cx, cy, 6, COL_PURPLE);
  dsp.drawCircle(cx, cy, 3, COL_PINK);
}

// Başlığı hafif glitch ile çiz + yanına logo
void drawGlitchTitle(const char* txt, int baseX, int baseY, uint16_t col) {
  dsp.setFont(&PressStart2P_Regular7pt7b);
  dsp.setTextColor(col);

  // Ghost gölge
  dsp.setCursor(baseX + rnd(-1,2), baseY + rnd(-1,2));
  dsp.print(txt);

  // Asıl yazı
  dsp.setTextColor(COL_PURPLE);
  dsp.setCursor(baseX, baseY);
  dsp.print(txt);

  // Yazı genişliğini ölç → logoyu hemen yanına koy
  int textW = dsp.textWidth(txt);
  int logoX = baseX + textW + 20;  // +20 boşluk
  int logoY = baseY - dsp.fontHeight()/2; // ortalama hizalama
  drawCardPortalLogo(logoX, logoY);
}

void runMinimalIntro() {
  const char* title = "CARDPORTAL";

  // Glitch animasyon
  for (int frame = 0; frame < 12; frame++) {
    dsp.fillScreen(COL_BG);

    drawGlitchTitle(title, 30, 60, (frame % 2 ? COL_PINK : COL_BLUE));
    drawScanlines();

    delay(120);
  }

  // Final sabit logo
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

void setup() {
  M5Cardputer.begin();
  dsp.setRotation(1);
  randomSeed(esp_timer_get_time());

  runMinimalIntro();
}

void loop() {}
