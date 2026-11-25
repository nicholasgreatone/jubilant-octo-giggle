#pragma once
// Environmental sensors. Reads values and returns numbers; never draws anything,
// so this module carries over to the ESP32-P4 unchanged.

#include <Arduino.h>

bool sensorsBegin();                          // true if the BMP280 answered
bool sensorsRead(float &tempF, float &altM);  // false if the sensor is missing
