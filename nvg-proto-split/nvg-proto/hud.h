#pragma once
// HUD layout and text: heading tape, heading numbers, sensor values.
// Draws onto whatever Adafruit_GFX target hudBegin() is given, so it does not
// depend on a particular display driver. Today that target is the lcd; on the
// ESP32-P4 it can be an off-screen overlay canvas instead, with no changes here.

#include <Arduino.h>
#include <Adafruit_GFX.h>

void hudBegin(Adafruit_GFX &target);   // choose where the HUD draws (call once)

void hudDrawStatic();                  // parts drawn once: the heading tape
void drawHeadingLabels(int hdg);       // the 3 heading numbers under the tape
void hudDrawSensorData(bool ok, float tempF, float altM);   // values, or "NO BMP280"

// Draws one value padded to 8 characters, e.g. "72.4 F  " (left) or " 123.4 m" (right)
void drawValue(int16_t x, int16_t y, float value, const char *unit, bool rightAlign);

// Keeps any angle in 0-359 (handles negatives, e.g. -10 -> 350)
int wrap360(int deg);
