#pragma once
#include "config.h"

enum Emote { EM_BOOT, EM_HAPPY, EM_SAD, EM_HUNGRY, EM_EAT, EM_PLAY, EM_TAP, EM_ALERT, EM_DEAD };

void soundInit();
void soundPlay(Emote e);  // non-blocking; respects volume / quiet hours
void soundUpdate();       // call every loop
void soundOff();          // stop and drive the buzzer pin LOW (also before sleep)
