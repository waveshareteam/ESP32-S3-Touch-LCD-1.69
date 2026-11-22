#pragma once
#include "config.h"

void uiInit();
void uiBootScreen();                  // shows detected modules (onboard + external) at boot
void uiDraw(uint32_t nowEpoch);
void uiTap(int x, int y);
bool uiInGame();
void uiMessage(const char *m, uint32_t ms = 2500);
void uiSetBrightness(uint8_t level);  // 0 detaches PWM and drives the backlight LOW
void uiScreenOff();                   // ST7789 SLPIN + backlight off
void uiScreenOn();
void uiGoHome();
