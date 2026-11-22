#pragma once
#include "config.h"

enum HapticFx { HX_TAP, HX_DOUBLE, HX_BUZZ, HX_ALERT };

bool hapticsRescan();  // true when the motor driver newly appeared (hot-plug); runs setup/calibration
void hapticsInit();  // LRA mode, library 6, auto-calibration (cached in NVS)
void hapticsPlay(HapticFx fx);
void hapticsSleep();
void hapticsWake();
bool hapticsPresent();
