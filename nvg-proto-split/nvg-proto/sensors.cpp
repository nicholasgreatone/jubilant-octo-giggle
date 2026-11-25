#include "sensors.h"
#include "config.h"
#include <Wire.h>               // I2C bus for the BMP280
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>    // pressure/temperature sensor driver

static Adafruit_BMP280 bmp;     // private to this file: nothing else touches the sensor
static bool bmpOk = false;      // true once the BMP280 responds at startup

bool sensorsBegin()
{
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
  return bmpOk;
}

bool sensorsRead(float &tempF, float &altM)
{
  if (!bmpOk) return false;
  tempF = bmp.readTemperature() * 1.8 + 32;          // Conversion to F
  altM  = bmp.readAltitude(SEALEVELPRESSURE_HPA);    // meters, from pressure
  return true;
}
