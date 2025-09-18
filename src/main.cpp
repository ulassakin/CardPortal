#include <Arduino.h>
#include <M5Cardputer.h>
#include "intro.h"
#include "evil_portal.h"
#include "menu.h"



void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  menu_setup();

  runMinimalIntro();   // Intro
  
  currentState = PRESS;
  
}

void loop() {
  if (currentState == RUNNING) {
    portal_loop_once();   // Captive portal aktif
  } else {
    menu_loop();          // Menü state machine
  } 
}
