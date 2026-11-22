#pragma once
#include "config.h"

struct Pet {
  float fullness = 80;   // 0..100 (100 = full)
  float happiness = 80;  // 0..100
  float health = 100;    // 0..100, 0 = dead
  uint32_t bornEpoch = 0;
  uint32_t lastEpoch = 0;  // RTC time of the last simulated moment
  uint32_t hatchEpoch = 0; // RTC time the egg hatches (stats are frozen until then)
  uint8_t alive = 1;
  uint8_t reserved[3] = {0, 0, 0};
};
extern Pet pet;

enum Mood { MOOD_HAPPY, MOOD_OK, MOOD_HUNGRY, MOOD_SAD, MOOD_SICK, MOOD_DEAD };

void petLoad(uint32_t nowEpoch);               // load from NVS and do offline catch-up
void petSave();
void petTick(uint32_t nowEpoch, bool sleeping);  // advance using the latest temperature
bool petFeed();
void petPlay(int score);
void petReset(uint32_t nowEpoch);
Mood petMood();

#define HATCH_SECONDS 60
bool petIsEgg(uint32_t nowEpoch);
float petHatchProgress(uint32_t nowEpoch);  // 0..1 while hatching
#if POCKET_PET_DEBUG
void petDebugWarp(uint32_t seconds);  // simulate `seconds` of awake time immediately
#endif
