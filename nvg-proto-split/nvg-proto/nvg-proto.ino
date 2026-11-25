// NVG HUD prototype: heading tape + BMP280 temperature/altitude on a 240x240 GC9A01 round display.
// This file only runs the show: setup() and loop(). Everything else lives in its own module:
//   config.h            pins, colours, positions, timings
//   display.h/.cpp      lcd object, start-up, boot splash, drawBitmapScaled()
//   hud.h/.cpp          heading tape, heading numbers, sensor values
//   sensors.h/.cpp      BMP280 set-up and reading
//   splash.h, nav_tape.h  bitmap data

#include "config.h"
#include "display.h"
#include "hud.h"
#include "sensors.h"

int heading = 274;      // fake magnetic heading; replace with sensor data later

// Reads the sensors and redraws the values (or the warning if the BMP280 is missing)
static void updateSensorData()
{
  float tempF = 0, altM = 0;
  bool ok = sensorsRead(tempF, altM);
  hudDrawSensorData(ok, tempF, altM);
}

void setup()
{
  Serial.begin(9600);
  displayInit();

  splashShow();         // logo goes up...
  sensorsBegin();       // ...sensor starts while it is on screen...
  splashFinish();       // ...then wait out SPLASH_MS and clear

  hudBegin(lcd);        // HUD draws straight to the display
  hudDrawStatic();      // static parts drawn once; only the numbers are redrawn in loop()
  drawHeadingLabels(heading);
  updateSensorData();
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
    updateSensorData();
  }
}
