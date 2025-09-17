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

String htmlFiles[20];
int htmlFileCount = 0;

String chosenHTML = "";

bool cursorVisible = true;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 500; // ms

M5GFX &dsp = M5Cardputer.Display;
bool pressDrawn = false;

int selectedHtmlIndex = 0;

AppState currentState;

M5Canvas portalCanvas(&M5Cardputer.Display);


// ---- Captive Portal HTML Templates ----

// 1. Basit login ekranı
const char* html_login = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Free WiFi Login</title>
  <style>
    body { font-family: Arial; text-align: center; background:#111; color:#eee; }
    .box { margin-top:50px; padding:20px; background:#222; border-radius:8px; display:inline-block; }
    input { padding:8px; margin:5px; width:200px; }
    button { padding:10px 20px; background:#4CAF50; color:white; border:none; cursor:pointer; }
  </style>
</head>
<body>
  <h2>Welcome to Free WiFi</h2>
  <div class="box">
    <form action="/login" method="POST">
      <input type="text" name="user" placeholder="Username"><br>
      <input type="password" name="pass" placeholder="Password"><br>
      <button type="submit">Login</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// 2. Küçük anket formu
const char* html_survey = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Quick Survey</title>
  <style>
    body { font-family: Verdana; background:#f0f0f0; text-align:center; }
    .form { margin-top:50px; }
    input, select { padding:8px; margin:5px; width:220px; }
    button { margin-top:10px; padding:10px 20px; }
  </style>
</head>
<body>
  <h2>Before you continue, please answer:</h2>
  <div class="form">
    <form action="/survey" method="POST">
      <input type="text" name="age" placeholder="Your Age"><br>
      <select name="device">
        <option value="">Device Type</option>
        <option>Phone</option>
        <option>Laptop</option>
        <option>Tablet</option>
      </select><br>
      <button type="submit">Submit</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// 3. Duyuru ekranı
const char* html_notice = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Important Notice</title>
  <style>
    body { font-family: Tahoma; background:#fff8e1; text-align:center; color:#333; }
    .box { margin:50px auto; padding:20px; border:1px solid #ccc; width:300px; }
  </style>
</head>
<body>
  <div class="box">
    <h2>Attention</h2>
    <p>This network is monitored. By continuing, you agree to our terms.</p>
    <form action="/accept" method="POST">
      <button type="submit">I Accept</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

// 4. Minimal karşılama
const char* html_welcome = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Welcome</title>
  <style>
    body { display:flex; justify-content:center; align-items:center;
           height:100vh; background:#282c34; color:white; font-family:sans-serif; }
  </style>
</head>
<body>
  <h1>Welcome to CardPortal</h1>
</body>
</html>
)rawliteral";



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

static String htmlPathForIndex(int index) {
    switch(index) {
        case 0: return "/login.html";
        case 1: return "/survey.html";
        case 2: return "/notice.html";
        case 3: return "/welcome.html";
        default: return "";
    }
}


static void writeTextFile(const String& path, const String& content) {
    SD.remove(path); // ensure truncate
    File f = SD.open(path, FILE_WRITE);
    if (!f) return;
    f.print(content);
    f.close();
}




void loadHTMLFromSD() {
    htmlFileCount = 0;
    for (int i = 0; i < 4; i++) {
        String filename = htmlPathForIndex(i);
        File file = SD.open(filename);
        if (file) {
            String content = "";
            while (file.available()) {
                content += (char)file.read();
            }
            file.close();
            content.trim();
            if (content.length() > 0) {
               htmlFiles[htmlFileCount++] = content;
            }
        }
    }
}

void drawHtmlList() {
    portalCanvas.fillScreen(BLACK);
    portalCanvas.setFont(&PressStart2P_Regular7pt7b);
    portalCanvas.setTextColor(WHITE, BLACK);

    const char* options[4] = {
        "Login Page",
        "Survey Page",
        "Notice Page",
        "Welcome Page"
    };

    for (int i = 0; i < 4; i++) {
        if (i == selectedHtmlIndex) {
            portalCanvas.setTextColor(BLACK, COL_PURPLE); // ters renk: seçili
        } else {
            portalCanvas.setTextColor(WHITE, BLACK);
        }
        portalCanvas.setCursor(20, 40 + i * 20);
        portalCanvas.print(options[i]);
    }

    portalCanvas.pushSprite(0,0);
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

    writeTextFile("/login.html", html_login);
    writeTextFile("/survey.html", html_survey);
    writeTextFile("/notice.html", html_notice);
    writeTextFile("/welcome.html", html_welcome);

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
            
            drawHtmlList();            
            if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
                auto keys = M5Cardputer.Keyboard.keysState();
                
                if (keys.enter) {
                    loadHTMLFromSD();
                    chosenHTML = htmlFiles[selectedHtmlIndex];
                    currentState = RUNNING;
                }

                else {
                    for (auto k : keys.word) {
                        if (k == 59 && selectedHtmlIndex > 0) {
                            selectedHtmlIndex--;
                            drawHtmlList();
                        }
                        else if (k == 46 && selectedHtmlIndex < 3) {
                            selectedHtmlIndex++;
                            drawHtmlList();
                        }
                    }
                }
            }
            break;
        }
    }
}