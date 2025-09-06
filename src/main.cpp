#include <Arduino.h>
#include <M5Cardputer.h>
#include "intro.h"
#include "evil_portal.h"

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  runMinimalIntro();   // açılış ekranı
  portal_begin();      // captive portal başlat
}

void loop() {
  portal_loop_once();  // captive portal döngüsü
}
