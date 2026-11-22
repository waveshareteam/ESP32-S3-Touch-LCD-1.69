#include "sound.h"
#include "board.h"
#include "settings.h"

struct Note {
  uint16_t f;  // Hz, 0 = rest
  uint16_t ms;
};

static const Note BOOT[] = {{523, 80}, {659, 80}, {784, 120}, {0, 0}};
static const Note HAPPY[] = {{988, 70}, {1319, 70}, {1568, 110}, {0, 0}};
static const Note SAD[] = {{523, 160}, {440, 160}, {330, 260}, {0, 0}};
static const Note HUNGRY[] = {{440, 90}, {0, 60}, {440, 90}, {0, 60}, {330, 200}, {0, 0}};
static const Note EAT[] = {{660, 50}, {0, 30}, {660, 50}, {0, 30}, {880, 90}, {0, 0}};
static const Note PLAY[] = {{784, 60}, {988, 60}, {784, 60}, {1175, 100}, {0, 0}};
static const Note TAP[] = {{1500, 25}, {0, 0}};
static const Note ALERT[] = {{1760, 120}, {0, 60}, {1760, 120}, {0, 0}};
static const Note DEAD[] = {{392, 250}, {330, 250}, {262, 250}, {196, 500}, {0, 0}};
static const Note *const TABLE[] = {BOOT, HAPPY, SAD, HUNGRY, EAT, PLAY, TAP, ALERT, DEAD};

static const Note *cur = nullptr;
static uint32_t noteEnd = 0;
static bool attached = false;

void soundOff() {
  cur = nullptr;
  if (attached) {
    ledcWrite(PIN_BUZZER, 0);
    ledcDetach(PIN_BUZZER);
    attached = false;
  }
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);  // no DC through the buzzer driver
}

void soundInit() { soundOff(); }

static bool allowed() {
  if (cfg.volume == 0) return false;
  if (cfg.quiet) {
    int h = hourOfDay(rtcNow());
    if (h >= 22 || h < 7) return false;
  }
  return true;
}

static void startNote() {
  if (!cur || cur->ms == 0) {
    soundOff();
    return;
  }
  // Duty as volume (10-bit, max 1023). Passive buzzers get no louder past ~25% and distort/clip there,
  // and a high duty also sags the 3V3 rail, so the top step is deliberately well below 50%.
  static const uint16_t duty[] = {0, 24, 96, 256};
  if (cur->f == 0) {
    if (attached) ledcWrite(PIN_BUZZER, 0);
  } else {
    if (!attached) attached = ledcAttach(PIN_BUZZER, cur->f, 10);
    else ledcChangeFrequency(PIN_BUZZER, cur->f, 10);
    ledcWrite(PIN_BUZZER, duty[cfg.volume]);
  }
  noteEnd = millis() + cur->ms;
}

void soundPlay(Emote e) {
  if (!allowed()) return;
  cur = TABLE[e];
  startNote();
}

void soundUpdate() {
  if (cur && (int32_t)(millis() - noteEnd) >= 0) {
    cur++;
    startNote();
  }
}
