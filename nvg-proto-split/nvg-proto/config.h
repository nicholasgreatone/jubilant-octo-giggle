#pragma once
// All tunable settings in one place: pins, colours, positions, timings.
// Only #defines and small constants live here, so any file can include it safely.

// ---- Display ----
#define SCR_WD   240
#define SCR_HT   240
#define TFT_CS   -1             // no CS pin: display CS tied to GND
#define TFT_DC    9             // data/command select
#define TFT_RST  10             // display reset

// ---- RGB565 UI colours ----
#define AMBER    0xFD20         // HUD elements + boot splash
#define HUD_BG   0x0000         // black text background (erases old digits)

//#define DARK_RED   0x7800
//#define PURPLE     0x8010
//#define MAGENTA    0xF81F
//#define DARKGREEN  0x0B01
//#define TITANIUM   0x9CF3
//#define SLATE_BLUE 0x6B90

// ---- Boot splash ----
// Position is worked out in display.cpp from the bitmap size (centred).
#define SPLASH_MS  2000         // how long the splash stays up

// ---- Nav bar heading tape (top-left corner, in pixels) ----
#define NAV_X  69               // (240 - 102) / 2 = horizontally centred
#define NAV_Y  10               // just inside the top of the round screen

// ---- Heading labels ----
#define TICK_STEP   10          // degrees between thick ticks
#define LABEL_ROW   13          // row inside the tape bitmap where the labels start
#define DEMO_TURN   1           // 1 = fake a slow turn, 0 = static heading
#define HEADING_MS  150         // how often the fake heading moves

// Label positions, matched to the tape's thick tick marks (used by hud.cpp):
// X position inside the bitmap, and how many ticks away from centre.
#define LABEL_TICK_X_LIST     { 10, 50, 91 }   // left, centre (caret), right
#define LABEL_TICK_OFFS_LIST  { -2,  0,  2 }   // -2 ticks, centre, +2 ticks

// ---- Sensor data (top-left corner, in pixels) ----
// Both values on one row: temperature left-aligned, altitude right-aligned.
#define DATA_Y     40
#define TEMP_X     34           // left value (temperature)
#define ALT_X      160          // right value (altitude): 8 chars x 6 px, ends at x 207
#define DATA_SIZE  1            // text size, same as the nav bar labels
#define SENSOR_MS  200          // how often to read the BMP280

// Local sea-level pressure in hPa. Altitude is only accurate if this matches today's weather report.
#define SEALEVELPRESSURE_HPA 1031.16
