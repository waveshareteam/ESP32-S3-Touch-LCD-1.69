#pragma once
#include "config.h"

struct SensorData {
  float tempC = NAN;
  float hum = NAN;
  float lux = NAN;
  bool hasTH = false;
  bool hasLux = false;
};
extern SensorData env;

void sensorsInit();
bool sensorsRescan();   // re-probe 0x38/0x10 (boards may be attached after boot)
void sensorsPoll();     // honours cfg.tempSensor and cfg.lightSensor; absent sensors are skipped quietly
void sensorsShutdown(); // VEML7700 power-save before sleep
bool sensorsTempKnown();
float tempNow();     // current temperature (debug override if set, else AHT10)
#if POCKET_PET_DEBUG
extern float dbgTempC;  // NAN = no override
#endif
bool lightUsable();  // light sensor enabled AND detected (required for auto brightness)
