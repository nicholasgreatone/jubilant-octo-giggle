#include "hud.h"
#include "config.h"
#include "nav_tape.h"           // tape data: included here only, so it is stored once

static Adafruit_GFX *gfx = nullptr;    // drawing target set by hudBegin()

// Label positions from config.h, stored once here
static const uint8_t LABEL_TICK_X[3]    = LABEL_TICK_X_LIST;
static const int8_t  LABEL_TICK_OFFS[3] = LABEL_TICK_OFFS_LIST;

void hudBegin(Adafruit_GFX &target)
{
  gfx = &target;
}

int wrap360(int deg)
{
  return ((deg % 360) + 360) % 360;
}

void hudDrawStatic()
{
  // One colour: 0 bits are left untouched, so whatever is behind shows through
  gfx->drawBitmap(NAV_X, NAV_Y, navTapeBitmap, NAV_WIDTH, NAV_HEIGHT, AMBER);
}

void drawHeadingLabels(int hdg)
{
  gfx->setTextSize(1);
  gfx->setTextColor(AMBER, HUD_BG);   // background colour overwrites the previous number

  for (uint8_t i = 0; i < 3; i++) {
    int value = wrap360(hdg + LABEL_TICK_OFFS[i] * TICK_STEP);   // e.g. 254 / 274 / 294

    char buf[4];
    snprintf(buf, sizeof(buf), "%03d", value);   // always 3 digits: 005, 090, 274

    // -7 centres the 17 px wide label on the tick
    gfx->setCursor(NAV_X + LABEL_TICK_X[i] - 7, NAV_Y + LABEL_ROW);
    gfx->print(buf);
  }
}

void drawValue(int16_t x, int16_t y, float value, const char *unit, bool rightAlign)
{
  char num[12];
  dtostrf(value, 1, 1, num);   // float to text, 1 decimal (AVR snprintf can't print floats)

  char txt[16];
  snprintf(txt, sizeof(txt), "%s %s", num, unit);   // "72.4 F"

  // Fixed 8-char width so a shorter value fully covers the old one
  char line[9];
  snprintf(line, sizeof(line), rightAlign ? "%8s" : "%-8s", txt);

  gfx->setCursor(x, y);
  gfx->print(line);
}

void hudDrawSensorData(bool ok, float tempF, float altM)
{
  gfx->setTextSize(DATA_SIZE);
  gfx->setTextColor(AMBER, HUD_BG);   // background colour erases the previous values

  if (!ok) {                          // sensor missing: show a warning, keep the HUD running
    gfx->setCursor(TEMP_X, DATA_Y);
    gfx->print("NO BMP280");
    return;
  }

  drawValue(TEMP_X, DATA_Y, tempF, "F", false);   // left side
  drawValue(ALT_X,  DATA_Y, altM,  "m", true);    // right side
}
