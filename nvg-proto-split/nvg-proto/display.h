#pragma once
// Display hardware: the lcd object, start-up, the boot splash, and drawing helpers.
// This is the only module that knows which display driver is used, so it is the
// one that gets replaced when the project moves to the ESP32-P4 (esp_lcd).

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "GC9A01A_AVR.h"        // round display driver

extern GC9A01A_AVR lcd;          // defined once, in display.cpp

void displayInit();              // start the panel and clear it to black
void displayClear();             // fill the whole screen with black

// Boot splash, split in two so other set-up (sensors) runs while it is on screen:
void splashShow();               // draw the logo and start the SPLASH_MS timer
void splashFinish();             // wait out the rest of SPLASH_MS, then clear the screen

// Draws a 1-bit PROGMEM bitmap stretched to dw x dh pixels (nearest-neighbour).
void drawBitmapScaled(int16_t x0, int16_t y0, const uint8_t *bmp,
                      uint16_t w, uint16_t h, uint16_t dw, uint16_t dh, uint16_t color);
