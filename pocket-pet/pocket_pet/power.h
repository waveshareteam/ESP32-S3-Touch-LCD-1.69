#pragma once
#include "config.h"

// Screen off + SLPIN, peripherals idle, light sleep with a slow periodic sensor wake.
// Returns when the user touches the screen (TP_INT) or presses BOOT.
void powerSleepUntilTouch();
void lowBatteryGuard();  // saves state and drops the power latch below ~3.3 V
