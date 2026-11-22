#include "pet.h"
#include "sensors.h"
#include <Preferences.h>

Pet pet;

static float clamp100(float v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }

// Advance the simulation by dt seconds at a fixed temperature. rate = 1.0 awake, 0.5 asleep/offline.
static void advance(uint32_t dt, float tempC, float rate) {
  const uint32_t CHUNK = 600;
  while (dt > 0 && pet.alive) {
    uint32_t s = dt < CHUNK ? dt : CHUNK;
    dt -= s;
    float h = s / 3600.0f * rate;
    bool cold = tempC < 16, freezing = tempC < 10, hot = tempC > 28, scorching = tempC > 32;
    bool comfy = tempC >= 20 && tempC <= 26;

    pet.fullness = clamp100(pet.fullness - h * (cold ? 5.0f : 4.0f));      // cold: eats faster
    pet.happiness = clamp100(pet.happiness - h * (hot ? 4.0f : 3.0f));    // hot: grumpier
    if (comfy) pet.happiness = clamp100(pet.happiness + h * 0.5f);

    float drain = 0;
    if (pet.fullness <= 0) drain += 3;
    else if (pet.fullness < 20) drain += 1;
    if (pet.happiness < 20) drain = drain * 1.5f + 0.5f;  // depressed: faster life drain
    if (freezing || scorching) drain += 2;
    else if (cold || hot) drain += 0.7f;
    float regen = (comfy && pet.fullness > 50 && pet.happiness > 50) ? 1.5f : 0;
    pet.health = clamp100(pet.health + h * (regen - drain));
    if (pet.health <= 0) pet.alive = 0;
  }
}

void petReset(uint32_t now) {
  pet = Pet();
  pet.bornEpoch = pet.lastEpoch = now;
  pet.hatchEpoch = now + HATCH_SECONDS;  // new pets start as an egg that hatches after 60 s
  petSave();
}

void petSave() {
  Preferences p;
  p.begin("pet", false);
  p.putBytes("p", &pet, sizeof(Pet));
  p.end();
}

// Advance to `now`, but never simulate the egg stage: the pet starts living at hatchEpoch.
static void catchUp(uint32_t now, float tempC, float rate) {
  if (pet.lastEpoch < pet.hatchEpoch) pet.lastEpoch = now < pet.hatchEpoch ? now : pet.hatchEpoch;
  if (now > pet.lastEpoch) advance(now - pet.lastEpoch, tempC, rate);
  pet.lastEpoch = now;
}

bool petIsEgg(uint32_t now) { return now < pet.hatchEpoch; }
float petHatchProgress(uint32_t now) {
  if (now >= pet.hatchEpoch) return 1.0f;
  return 1.0f - (pet.hatchEpoch - now) / (float)HATCH_SECONDS;
}

void petLoad(uint32_t now) {
  Preferences p;
  p.begin("pet", true);
  bool ok = p.getBytesLength("p") == sizeof(Pet);
  if (ok) p.getBytes("p", &pet, sizeof(Pet));
  p.end();
  if (!ok) {
    petReset(now);
    return;
  }
  // Offline catch-up: device was off/asleep, assume neutral 22 C at half rate. Clock going backwards = ignore.
  catchUp(now, 22.0f, 0.5f);
  petSave();
}

void petTick(uint32_t now, bool sleeping) {
  if (now <= pet.lastEpoch) return;
  float t = sensorsTempKnown() ? tempNow() : 22.0f;
  catchUp(now, t, sleeping ? 0.5f : 1.0f);
}

bool petFeed() {
  if (!pet.alive || pet.fullness > 95) return false;
  pet.fullness = clamp100(pet.fullness + 30);
  pet.happiness = clamp100(pet.happiness + 3);
  return true;
}

void petPlay(int score) {
  if (!pet.alive) return;
  pet.happiness = clamp100(pet.happiness + (score * 4 > 40 ? 40 : score * 4));
  pet.fullness = clamp100(pet.fullness - 3);
}

Mood petMood() {
  if (!pet.alive) return MOOD_DEAD;
  if (pet.health < 30) return MOOD_SICK;
  if (pet.fullness < 25) return MOOD_HUNGRY;
  if (pet.happiness < 25) return MOOD_SAD;
  if (pet.happiness > 70 && pet.fullness > 50) return MOOD_HAPPY;
  return MOOD_OK;
}

#if POCKET_PET_DEBUG
void petDebugWarp(uint32_t secs) {
  if (pet.hatchEpoch > pet.lastEpoch) {  // egg: just shorten the hatch timer
    uint32_t left = pet.hatchEpoch - pet.lastEpoch;
    pet.hatchEpoch -= secs < left ? secs : left;
    return;
  }
  pet.lastEpoch -= secs;
  petTick(pet.lastEpoch + secs, false);
}
#endif
