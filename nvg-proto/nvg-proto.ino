// NVG HUD prototype: heading tape + BMP280 temperature/altitude on a 240x240 GC9A01 round display.

#include <SPI.h>                // display bus
#include <Wire.h>               // I2C bus for the BMP280
#include <Adafruit_GFX.h>       // text and bitmap drawing
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>    // pressure/temperature sensor driver
#include <avr/pgmspace.h>       // read constant data from flash
#include "GC9A01A_AVR.h"        // round display driver
#include "bitmap.h"             // nav bar heading tape image

// ---- Display ----
#define SCR_WD 240
#define SCR_HT 240
#define TFT_CS  -1              // no CS pin: display CS tied to GND
#define TFT_DC   9              // data/command select
#define TFT_RST  10             // display reset
#define AMBER 0xFD20            // RGB565 color for all HUD elements

// Local sea-level pressure in hPa. Altitude is only accurate if this matches today's weather report.
#define SEALEVELPRESSURE_HPA 1031.16

// ---- Bitmap position (top-left corner, in pixels) ----
#define NAV_X  69               // (240 - 102) / 2 = horizontally centered
#define NAV_Y  10               // just inside the top of the round screen

// ---- Heading labels ----
#define TICK_STEP   10   // degrees between thick ticks
#define LABEL_ROW   13   // row inside the bitmap where the labels start
#define DEMO_TURN   1    // 1 = fake a slow turn, 0 = static heading
#define HEADING_MS  150  // how often the fake heading moves

// Which thick ticks get a label: x position inside the bitmap, and ticks away from center
const uint8_t LABEL_TICK_X[3]    = { 10, 50, 91 };   // left, center (caret), right
const int8_t  LABEL_TICK_OFFS[3] = { -2,  0,  2 };   // -2 ticks, center, +2 ticks


// ---- Sensor data positions (top-left corner, in pixels) ----
// Both values on the same row: temperature left-aligned, altitude right-aligned.
#define DATA_Y       40
#define TEMP_X       34    // left value (temperature)
#define ALT_X        160   // right value (altitude): 8 chars x 6 px wide, so it ends at x 207
#define DATA_SIZE    1     // text size, same as the nav bar labels
#define SENSOR_MS    200   // how often to read the BMP280

int  heading = 274;      // fake magnetic heading; replace with sensor data later
bool bmpOk   = false;    // true once the BMP280 responds at startup

GC9A01A_AVR lcd(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BMP280 bmp;

// Keeps any angle in 0-359 (handles negatives, e.g. -10 -> 350)
int wrap360(int deg)
{
  return ((deg % 360) + 360) % 360;
}

// Draws the 3 heading numbers under the nav bar's thick ticks
void drawHeadingLabels(int hdg)
{
  lcd.setTextSize(1);
  lcd.setTextColor(AMBER, BLACK);   // black background overwrites the previous number

  for (uint8_t i = 0; i < 3; i++) {
    int value = wrap360(hdg + LABEL_TICK_OFFS[i] * TICK_STEP);   // e.g. 254 / 274 / 294

    char buf[4];
    snprintf(buf, sizeof(buf), "%03d", value);   // always 3 digits: 005, 090, 274

    // -7 centers the 17 px wide label on the tick
    lcd.setCursor(NAV_X + LABEL_TICK_X[i] - 7, NAV_Y + LABEL_ROW);
    lcd.print(buf);
  }
}

// Draws one value padded to 8 characters, e.g. "72.4 F  " (left) or " 123.4 m" (right)
void drawValue(int16_t x, int16_t y, float value, const char* unit, bool rightAlign)
{
  char num[12];
  dtostrf(value, 1, 1, num);   // float to text, 1 decimal (AVR snprintf can't print floats)

  char txt[12];
  snprintf(txt, sizeof(txt), "%s %s", num, unit);   // "72.4 F"

  // Fixed 8-char width so a shorter value fully covers the old one
  char line[9];
  snprintf(line, sizeof(line), rightAlign ? "%8s" : "%-8s", txt);

  lcd.setCursor(x, y);
  lcd.print(line);
}

// Reads the BMP280 and redraws temperature and altitude
void drawSensorData()
{
  lcd.setTextSize(DATA_SIZE);
  lcd.setTextColor(AMBER, BLACK);   // black background erases the previous values

  if (!bmpOk) {                     // sensor missing: show a warning, keep the HUD running
    lcd.setCursor(TEMP_X, DATA_Y);
    lcd.print("NO BMP280");
    return;
  }

  float fahrenheit = bmp.readTemperature() * 1.8 + 32;        // Conversion to F
  float altitude   = bmp.readAltitude(SEALEVELPRESSURE_HPA);  // meters, from pressure

  drawValue(TEMP_X, DATA_Y, fahrenheit, "F", false);   // left side
  drawValue(ALT_X,  DATA_Y, altitude,   "m", true);    // right side
}

void setup()
{
  Serial.begin(9600);
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);

  // Try both possible I2C addresses (set by the board's SDO pin)
  bmpOk = bmp.begin(0x76) || bmp.begin(0x77);
  if (bmpOk) {
    // Oversampling + filtering for steady readings; new measurement about every 0.5 s
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,     // temperature
                    Adafruit_BMP280::SAMPLING_X16,    // pressure
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
  } else {
    Serial.println("BMP280 not detected!");
  }

  // Static parts drawn once; only the numbers are redrawn in loop()
  lcd.drawBitmap(NAV_X, NAV_Y, bitmap, NAV_WIDTH, NAV_HEIGHT, AMBER);   // one color: 0 bits left untouched
  drawHeadingLabels(heading);
  drawSensorData();
}

void loop()
{
  unsigned long now = millis();

  // Fake heading: +1 degree every HEADING_MS
#if DEMO_TURN
  static unsigned long lastTurn = 0;
  if (now - lastTurn >= HEADING_MS) {
    lastTurn = now;
    heading = wrap360(heading + 1);
  }
#endif

  // Redraw labels only when the heading changes
  static int lastHeading = heading;
  if (heading != lastHeading) {
    drawHeadingLabels(heading);
    lastHeading = heading;
  }

  // Sensor read + redraw every SENSOR_MS (millis timers, no delay)
  static unsigned long lastSensor = 0;
  if (now - lastSensor >= SENSOR_MS) {
    lastSensor = now;
    drawSensorData();
  }
}
