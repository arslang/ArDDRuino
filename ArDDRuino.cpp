//
//  ArDDRuino.cpp
//  
//

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include "lcd_image.h"
#include <math.h>
#include <avr/pgmspace.h>

#include "NoteSprite.h"
#include "songs.h"

// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

// joystick pins
#define VERT     0  // analog input
#define HORIZ    1  // analog input
#define SEL      9  // digital input

// button pins
#define BTNPIN1 31
#define BTNPIN2 33
#define BTNPIN3 35
#define BTNPIN4 37

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Sd2Card card;

struct Joystick {
    int delta_vert;
    int delta_horiz;
    boolean pushed;
    int pushcount;
    Joystick() : pushcount(-1) { }
};

struct Vector {
    int x;
    int y;
    Vector() : x(0), y(0) { }
};

int btnPins[] = {BTNPIN1, BTNPIN2, BTNPIN3, BTNPIN4};
int btnState[] = {0, 0, 0, 0};
int lastBtnState[] = {0, 0, 0, 0};

struct Button {
    boolean pushed;
};

Button Buttons[4];

Joystick joystick;
NoteSprite sprites[10];
Vector cursor, old_cursor;

boolean shouldExitState = false;

void initialization();

void loadPlayState();
void loadMenuState();
void loadScoreState();
void runMenuState();
void updateScore();
void updateBars();
void updatePlayState(unsigned long dt);
void renderPlayState();
void updateScoreState(unsigned long dt);
void renderScoreState();
void playSong(char *songName);

uint16_t color(int red, int green, int blue);

void animateBalls();
int getBallY(int i, int t);

unsigned long time = micros();
unsigned long game_time = 0;

int main() {
    
    initialization();
    
    typedef enum {MENUSTATE, PLAYSTATE, SCORESTATE} GameState;
    GameState state = MENUSTATE;
    
    int vertical, horizontal;
    int init_joystick_vert, init_joystick_horiz;
    init_joystick_vert = analogRead(VERT);
    init_joystick_horiz = analogRead(HORIZ);
    int lastSelState = 0, selState = 0;

    cursor.x = 64;
    cursor.y = 80;

    unsigned long dt;
    unsigned long counter = 0;

    while (true) {
        
        vertical = analogRead(VERT);      // will be 0-1023
        horizontal = analogRead(HORIZ);   // will be 0-1023
        selState = digitalRead(SEL);      // HIGH if not pressed, LOW otherwise
        
        //Compute the change in the joystick.
        joystick.delta_vert = vertical - init_joystick_vert;
        joystick.delta_horiz = horizontal - init_joystick_horiz;
        joystick.pushed = false;
        
        if (selState != lastSelState) {
            if (selState == HIGH) {
                joystick.pushed = true;
                ++joystick.pushcount;
            }
        }
        lastSelState = selState;
        
        unsigned long currentTime = micros();
        dt = currentTime - time;
        time = currentTime;

        Serial.println(shouldExitState);
        Serial.println(state);
        
        // STATES
        
        if (state == MENUSTATE) {
            if (counter == 0) loadMenuState();
            runMenuState();
            ++counter;
            shouldExitState = false;
            if (joystick.pushed == true && joystick.pushcount > 0) shouldExitState = true;
            if (shouldExitState) {
                counter = 0;
                state = PLAYSTATE;
            }
        }

        if (state == PLAYSTATE) {
            if (counter == 0) loadPlayState();
            updatePlayState(dt);
            renderPlayState();
            ++counter;
            game_time += dt;
            shouldExitState = false;
            if (joystick.pushed == true && joystick.pushcount > 0) shouldExitState = true;
            if (shouldExitState && game_time > 10000000) {
                counter = 0;
                state = SCORESTATE;
            }
        }

        if (state == SCORESTATE) {
            if (counter == 0) loadScoreState();
            ++counter;
            updateScoreState(dt);
            renderScoreState();
            shouldExitState = false;
            if (joystick.pushed == true && joystick.pushcount > 0) shouldExitState = true;
            if (shouldExitState) {
                counter = 0;
                state = SCORESTATE;
            }
        }
    }
    
    Serial.end();
    Serial3.end();
    
    return 0;
}

void initialization() {
    init();
    tft.initR(INITR_BLACKTAB); // initialize a ST7735R chip, black tab

    Serial.begin(9600);
    Serial3.begin(9600);

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed!");
    }
    Serial.println("OK!");
    // Some more initialization
    Serial.print("Doing raw initialization...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        Serial.println("failed!");
        while (true) {} // something is wrong
    }
    else {
        Serial.println("OK!");
    }

    // Initialize Joystick
    pinMode(SEL, INPUT);
    digitalWrite(SEL, HIGH);

    // clear to black
    tft.fillScreen(tft.Color565(0x00, 0x00, 0x00));

    //audio.speakerPin = 11;
    //audio.play("hello.wav");
}


// MENU STATE

void loadMenuState() {
    tft.setCursor(5, 50);
    tft.setTextColor(0x780F);
    tft.setTextSize(2);
    tft.print("ArDDRuino!");
    tft.setCursor(20, 70);
    tft.setTextColor(0xFD20);
    tft.setTextSize(0.6);
    tft.print("Click to begin");
}

void runMenuState() {
    /*old_cursor.x = cursor.x;
    old_cursor.y = cursor.y;
    tft.fillRect(old_cursor.x, old_cursor.y, 3, 3, 0x0);
    cursor.x = cursor.x + joystick.delta_horiz / 100;
    cursor.y = cursor.y + joystick.delta_vert / 100;
    tft.fillRect(cursor.x, cursor.y, 3, 3, 0xF800);*/
    animateBalls();
}


const int NUMBALLS = 20;
Vector balls[NUMBALLS];
int frame = 0;
const float BALLHEIGHT = 30;
void animateBalls() {
    
    for (int i = 0; i < NUMBALLS; ++i)
    {
        tft.fillCircle(balls[i].x, balls[i].y, 1, 0x0);
        balls[i].x = 7 * i;
        balls[i].y = getBallY(i, frame);
        tft.fillCircle(balls[i].x, balls[i].y, 1, 0x03EF);

    }
    frame += 3;
    
}

int getBallY(int i, int frame) {
    return 100.0 + BALLHEIGHT / 2.0 * (1.0 + sin(fmod((frame * (float(i) / 500.0 + 0.02)), 2.0*3.1415926)));
}

// END MENU STATE

// PLAY STATE



const int NUMCIRCLES = 20;
const int BARS = 4;
NoteSprite Circles[NUMCIRCLES];

const int TAPZONE_ABOVE = 140;
const int TAPZONE_BELOW = 155;
int Score;
int conScore;
int multiplier;
int oldMultiplier;
int barColor;

unsigned long nextEventTime;
int eventIndex;
boolean newEvent;



void loadPlayState() {

    //INITIALIZE PLAYSTATE BACKGROUND
    tft.fillScreen(tft.Color565(0x00,0x00,0x00));

    tft.fillRect(0,TAPZONE_ABOVE - 2,128,2,tft.Color565(0x00,0xFF,0x00));
    tft.fillRect(0,TAPZONE_BELOW - 2,128,2,tft.Color565(0xFF,0x00,0x00));

    barColor = 0xFFFF;
    for (int i = 1; i < BARS + 1; ++i) {
        tft.fillRect(i*25 - 5,0,11,160,barColor);
    }
    
    nextEventTime = 0;
    eventIndex = 0;
    nextEventTime = pgm_read_dword(&song1[eventIndex]);
    
    Score = 0;
    conScore = 0;
    int multiplier;
    int oldMultiplier;
    updateScore();
    
    playSong("Numb.wav");
}


int note;
void updatePlayState(unsigned long dt) {
    //Serial.print("game time"); Serial.println(game_time);
    //Serial.print("next event time"); Serial.println(nextEventTime);
    
    if (game_time > nextEventTime) {
        newEvent = true;
    }
    
    if (newEvent) { 
        //Serial.println("new event");
        unsigned long eventTime = nextEventTime;
        newEvent = false;
        while (pgm_read_dword(&song1[eventIndex]) == eventTime) {
            note = pgm_read_dword(&song1[eventIndex+1]);
            eventIndex += 2;
            nextEventTime = pgm_read_dword(&song1[eventIndex]);
            for (int i = 0; i < NUMCIRCLES; ++i) {
                if (Circles[i].onScreen == false) {
                    Circles[i].note = note;
                    Circles[i].onScreen = true;
                    Circles[i].setY(-5);
                    Circles[i].setX(Circles[i].note * 25);
                    
                    break;
                }
            }
        }
    }
    
    for (int i = 0; i < 4; ++i) {
        Buttons[i].pushed = false;
        btnState[i] = digitalRead(btnPins[i]);
        if (btnState[i] != lastBtnState[i]) {
            if (btnState[i] == HIGH) {
                Buttons[i].pushed = true;
            }
        }
    
    }

    for (int i = 0; i < NUMCIRCLES; ++i) {
        if (Circles[i].onScreen == true) {
            tft.fillCircle(Circles[i].getX(), Circles[i].getY() - 5, Circles[i].RADIUS + 1, barColor);
            Circles[i].update(dt);
            if (Circles[i].getY() > (170 + Circles[i].RADIUS)) {
                Circles[i].onScreen = false;
            }
            if (Circles[i].getY() > TAPZONE_ABOVE && Circles[i].getY() < TAPZONE_BELOW && Buttons[Circles[i].note -1].pushed == true) {
                Circles[i].onScreen = false;
                tft.fillCircle(Circles[i].getX(), Circles[i].getY(), Circles[i].RADIUS + 2, barColor);
                tft.fillRect(110,5,10,10,0x00);
                Score++;
                conScore++;
                multiplier = conScore / 5;
                updateScore();
                
            }   
            if (Circles[i].getY() > TAPZONE_BELOW) {     
                conScore = 0;
            }
        }
    }
    
    if (oldMultiplier != multiplier) {
        updateBars();
        oldMultiplier = multiplier;
    }

        
    
//    Serial.print("dt: "); Serial.println(dt);
//    Serial.print("y: "); Serial.println(Circles[0].getY());
    
}

void updateBars() {
    
    if (multiplier == 0) {
       barColor = 0xFFFF;
    } else if (multiplier == 1) { barColor = 0xFFCCCC;
    } else if (multiplier == 2) { barColor = 0xFF8080;
    } else if (multiplier == 3) { barColor = 0xFF4D4D;
    } else if (multiplier == 4) { barColor = 0xFF0000;
    }
    for (int i = 1; i < BARS + 1; ++i) {
        tft.fillRect(i*25 - 5,0,11,160,barColor);
    }
}
    

void renderPlayState() {

    for (int i = 0; i < NUMCIRCLES; ++i) {
        if (Circles[i].onScreen == true) {
            tft.fillCircle(Circles[i].getX(), Circles[i].getY(), Circles[i].RADIUS, 0x0);
        }
    }
}

void updateScore() {
    //Display score on screen.
    tft.setCursor(110, 5);
    tft.setTextColor(0xFFFF);
    tft.setTextSize(1);
    tft.print(Score);
}

void playSong(char *songName) {
    Serial3.write(10);

}

// END PLAY STATE

void loadScoreState() {
    tft.fillScreen(tft.Color565(0x00,0x00,0x00));
    tft.setCursor(17, 50);
    tft.setTextColor(0x780F);
    tft.setTextSize(2);
    tft.print("Score: ");
    tft.setCursor(90, 50);
    tft.print(Score);
    tft.setCursor(8, 70);
    tft.setTextColor(0xFD20);
    tft.setTextSize(0.6);
    tft.print("Click to try again!");
}

void updateScoreState(unsigned long dt) {

}

void renderScoreState() {

}

uint16_t color(int red, int green, int blue) {
    return (((31 * (red + 4)) / 255) << 11) |
        (((63 * (green + 2)) / 255) << 5) |
        ((31 * (blue + 4)) / 255);
}