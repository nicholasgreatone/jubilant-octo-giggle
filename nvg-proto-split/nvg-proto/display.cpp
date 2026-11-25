#include "display.h"
#include "config.h"
#include "splash.h"             // logo data: included here only, so it is stored once

GC9A01A_AVR lcd(TFT_CS, TFT_DC, TFT_RST);   // the one and only lcd object

static unsigned long splashStart = 0;       // when the splash went up

void displayInit()
{
  lcd.init(SCR_WD, SCR_HT);
  displayClear();
}

void displayClear()
{
  lcd.fillScreen(BLACK);
}

void splashShow()
{
  const int16_t x = (SCR_WD - SPLASH_WIDTH)  / 2;   // 0: centred
  const int16_t y = (SCR_HT - SPLASH_HEIGHT) / 2;   // 7: centred
  lcd.drawBitmap(x, y, splashBitmap, SPLASH_WIDTH, SPLASH_HEIGHT, AMBER);
  splashStart = millis();
}

void splashFinish()
{
  while (millis() - splashStart < SPLASH_MS) {}     // hold for the rest of SPLASH_MS
  displayClear();
}

void drawBitmapScaled(int16_t x0, int16_t y0, const uint8_t *bmp,
                      uint16_t w, uint16_t h, uint16_t dw, uint16_t dh, uint16_t color)
{
  uint16_t bytesPerRow = (w + 7) / 8;
  for (uint16_t sy = 0; sy < h; sy++) {
    int16_t ya = y0 + (uint32_t)sy * dh / h;          // top of this row on screen
    int16_t yb = y0 + (uint32_t)(sy + 1) * dh / h;    // top of the next row
    int16_t runStart = -1;                            // first lit pixel of the current run
    for (uint16_t sx = 0; sx <= w; sx++) {
      bool on = (sx < w) &&
                (pgm_read_byte(&bmp[sy * bytesPerRow + sx / 8]) & (0x80 >> (sx & 7)));
      if (on && runStart < 0) runStart = sx;          // run starts
      if (!on && runStart >= 0) {                     // run ends: draw it as one rectangle
        int16_t xa = x0 + (uint32_t)runStart * dw / w;
        int16_t xb = x0 + (uint32_t)sx * dw / w;
        lcd.fillRect(xa, ya, xb - xa, yb - ya, color);
        runStart = -1;
      }
    }
  }
}
