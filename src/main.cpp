#include <Arduino.h>
#include <M5Cardputer.h>
#include "intro.h"
#include "evil_portal.h"

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  runMinimalIntro();   // Intro
  portal_begin();      // Start the captive portal
}

void loop() {
  portal_loop_once();  // Captive Portal loop
}
