#pragma once
#include <Arduino.h>

struct Settings {
  bool tempSensor = true;       // poll AHT10 (temperature/humidity)
  bool lightSensor = true;      // poll VEML7700 (light)
  uint8_t volume = 2;           // 0 = mute, 1..3
  bool haptics = true;
  bool autoBright = true;
  bool quiet = true;            // mute sound 22:00-07:00
  uint8_t brightness = 200;     // manual level
  uint16_t awakePollS = 15;
  uint16_t sleepPollS = 300;    // slow poll while light-sleeping
  uint16_t screenTimeoutS = 30;
};
extern Settings cfg;
void settingsLoad();
void settingsSave();
