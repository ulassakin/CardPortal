#include "M5Cardputer.h"
#include "M5GFX.h"
#include <SPI.h>
#include <SD.h>
#include "menu.h"
#define COL_BG     BLACK
#define COL_PURPLE 0x780F
#define COL_PINK   0xF81F
#define COL_BLUE   0x07FF
#define COL_WHITE  WHITE


#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

#include "fonts/PressStart2P_Regular7pt7b.h"

//enum AppState {
  //  PRESS,
    //ENTER_SSID,
//    ENTER_PASS,
  //  SELECT_HTML,
   // RUNNING,
//};

String SSID = "";
String pass = "";
int cursorPosition = 0;

String currentText = "";

bool cursorVisible = true;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 500; // ms

M5GFX &dsp = M5Cardputer.Display;
bool pressDrawn = false;

AppState currentState;

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

void drawWithCursor() {
    portalCanvas.fillScreen(BLACK);
    portalCanvas.setFont(&PressStart2P_Regular7pt7b);
    

    const int startX = 0;
    const int startY = 0;

    String header;

    if (currentState == ENTER_SSID) {
        header = "Enter SSID:";
    } 
    else if (currentState == ENTER_PASS) {
        header = "Enter Password:";
    } 
    else if (currentState == SELECT_HTML) {
        header = "Select HTML\n";
    } 
    else {
        header = "";
    }


    portalCanvas.setCursor(startX, startY);
    portalCanvas.setTextColor(WHITE, BLACK);
    portalCanvas.print(header);
    portalCanvas.print(currentText);

    // Monospace cursor calculation
    int charWidth = portalCanvas.textWidth("A");
    int fontHeight = portalCanvas.fontHeight();
    int lineWidth = portalCanvas.width();
    int charsPerLine = lineWidth / charWidth;

    int totalChars = header.length() + cursorPosition;
    int lineNumber = totalChars / charsPerLine;
    int lineOffset = totalChars % charsPerLine;

    int cursorX;
    int cursorY;

    if (lineNumber >= 8 ){
        cursorX = lineOffset * charWidth;
        cursorY = 7 * fontHeight + 5;
    } else {
        cursorX = startX + lineOffset * charWidth;
        cursorY = startY + lineNumber * fontHeight;
    }

    if (cursorVisible) {
        portalCanvas.drawLine(cursorX, cursorY, cursorX, cursorY + fontHeight, WHITE);
    }
    portalCanvas.pushSprite(0, 0);
}

static void writeTextFile(const String& path, const String& content) {
    SD.remove(path); // ensure truncate
    File f = SD.open(path, FILE_WRITE);
    if (!f) return;
    f.print(content);
    f.close();
}



void menu_setup(){
    portalCanvas.createSprite(dsp.width(), dsp.height());

    Serial.begin(115200);

    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        Serial.println("SD init failed (card missing or init error). Running without SD.");
    } else {
        Serial.println("SD init OK");
        
    }
}

void menu_loop() {
    M5Cardputer.update();

    if (currentState == ENTER_PASS || currentState == ENTER_SSID) {
        if (millis() - lastBlink >= blinkInterval) {
            cursorVisible = !cursorVisible;
            lastBlink = millis();
            drawWithCursor();
        }
    }

    switch (currentState) {
        case PRESS: {
            if (!pressDrawn) {   // sadece ilk kez girince çiz
                drawPress();
                pressDrawn = true;
            }

            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                auto keys = M5Cardputer.Keyboard.keysState();
                if (keys.enter) {
                    currentState = ENTER_SSID;
                    drawWithCursor();
                }
            }

            break;
        }

        case ENTER_SSID: {
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                auto keys = M5Cardputer.Keyboard.keysState();
                if(keys.enter){
                    if (currentText.length() > 0) {
                        SSID = currentText;
                        currentText = "";
                        cursorPosition = 0;
                        currentState = ENTER_PASS;
                    }
                }

                else if (keys.del && currentText.length() > 0) {
                    if (cursorPosition > 0) {
                        currentText.remove(cursorPosition - 1, 1);
                        cursorPosition--;
                    }
                    drawWithCursor();
                }

                else {
                    for (auto k : keys.word) {
                        if (k == 44 & cursorPosition > 0) {
                            cursorPosition--;
                            drawWithCursor();
                        }
                        else if (k == 47 && (cursorPosition < currentText.length())) {
                            cursorPosition++;
                            drawWithCursor();
                        }
                        else if (k >= 32 && k <= 126) {
                            String left  = currentText.substring(0, cursorPosition);
                            String right = currentText.substring(cursorPosition);
                            currentText = left + (char)k + right;
                            cursorPosition++;
                            drawWithCursor();
                        }
                    }
                }
            }
            break;
        }

        case ENTER_PASS: {
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                auto keys = M5Cardputer.Keyboard.keysState();
                if(keys.enter){
                    if (currentText.length() > 0) {
                        pass = currentText;
                        currentText = "";
                        cursorPosition = 0;
                        currentState = SELECT_HTML;
                        drawWithCursor();
                    }
                }

                else if (keys.del && currentText.length() > 0) {
                    if (cursorPosition > 0) {
                        currentText.remove(cursorPosition - 1, 1);
                        cursorPosition--;
                    }
                    drawWithCursor();
                }

                else {
                    for (auto k : keys.word) {
                        if (k == 44 & cursorPosition > 0) {
                            cursorPosition--;
                            drawWithCursor();
                        }
                        else if (k == 47 && (cursorPosition < currentText.length())) {
                            cursorPosition++;
                            drawWithCursor();
                        }
                        else if (k >= 32 && k <= 126) {
                            String left  = currentText.substring(0, cursorPosition);
                            String right = currentText.substring(cursorPosition);
                            currentText = left + (char)k + right;
                            cursorPosition++;
                            drawWithCursor();
                        }
                    }
                }
            }
            break;
        }

        case SELECT_HTML: {
            break;
        }
    }
}