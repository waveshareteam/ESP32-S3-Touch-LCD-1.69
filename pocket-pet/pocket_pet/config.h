#pragma once
#include <Arduino.h>
#include "pin_config.h"

// Board revision V2.1 pins (confirmed). V1 differs: buzzer 33, SYS_EN 35, SYS_OUT 36, RTC_INT 41.
// Debug menu (Settings > Debug) for editing pet stats/temperature while testing.
// Set to 0 (or build with -DPOCKET_PET_DEBUG=0) for a release build: the menu is compiled out.
#ifndef POCKET_PET_DEBUG
#define POCKET_PET_DEBUG 1
#endif

#define PIN_BUZZER 42
#define PIN_SYS_EN 41
#define PIN_SYS_OUT 40
#define PIN_RTC_INT 39
#define PIN_BAT_ADC 1

#define ADDR_TOUCH 0x15
#define ADDR_RTC 0x51
#define ADDR_VEML 0x10
#define ADDR_AHT 0x38
#define ADDR_DRV 0x5A

#define C565(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

#if ARDUINO_USB_CDC_ON_BOOT
#define LOG Serial
#else
#include "HWCDC.h"
extern HWCDC USBSerial;
#define LOG USBSerial
#endif
