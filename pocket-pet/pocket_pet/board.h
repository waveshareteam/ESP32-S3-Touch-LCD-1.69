#pragma once
#include "config.h"

void boardInit();  // buzzer LOW, SYS_EN HIGH, single Wire.begin(11,10), touch reset, RTC init

bool i2cProbe(uint8_t addr);
bool i2cWriteRaw(uint8_t addr, const uint8_t *d, size_t n);
bool i2cReadRaw(uint8_t addr, uint8_t *d, size_t n);
bool i2cWriteReg(uint8_t addr, uint8_t reg, const uint8_t *d, size_t n);
bool i2cWrite8(uint8_t addr, uint8_t reg, uint8_t v);
bool i2cReadReg(uint8_t addr, uint8_t reg, uint8_t *d, size_t n);

bool touchGet(int &x, int &y);  // true while a finger is down

uint32_t rtcNow();  // seconds since 1970 of the RTC's local clock
void rtcSet(uint32_t epoch);
int hourOfDay(uint32_t epoch);
void dateParts(uint32_t epoch, int &y, int &m, int &d, int &weekday);  // weekday 0 = Sunday
void timeParts(uint32_t epoch, int &h, int &m);

float batteryVolts();
void powerLatchOff();  // drop SYS_EN (battery-only: powers the board off)
