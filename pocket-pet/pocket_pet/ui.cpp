#include "ui.h"
#include <Arduino_GFX_Library.h>
#include "board.h"
#include "game.h"
#include "haptics.h"
#include "pet.h"
#include "sensors.h"
#include "settings.h"
#include "sound.h"
#include "sprite.h"

static Arduino_DataBus *bus;
static Arduino_GFX *panel;
static Arduino_Canvas *cv;  // full-frame off-screen buffer (PSRAM) -> flicker-free
static bool blAttached = false;

enum Screen { S_PET, S_SETTINGS, S_GAME, S_DEBUG };
static Screen scr = S_PET;
static char msg[32];
static uint32_t msgUntil = 0;
static uint32_t lastTapMs = 0;

void uiSetBrightness(uint8_t level) {
  if (level == 0) {
    if (blAttached) ledcDetach(LCD_BL);
    blAttached = false;
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, LOW);
    return;
  }
  if (!blAttached) blAttached = ledcAttach(LCD_BL, 5000, 8);
  ledcWrite(LCD_BL, level);
}

void uiInit() {
  bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
  panel = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 20);
  cv = new Arduino_Canvas(LCD_WIDTH, LCD_HEIGHT, panel);
  panel->begin();
  cv->begin(GFX_SKIP_OUTPUT_BEGIN);
  cv->fillScreen(0);
  cv->flush();
  uiSetBrightness(cfg.brightness);
}

void uiScreenOff() {
  bus->sendCommand(0x28);  // display off
  bus->sendCommand(0x10);  // SLPIN
  uiSetBrightness(0);
}

void uiScreenOn() {
  bus->sendCommand(0x11);  // SLPOUT
  delay(120);
  bus->sendCommand(0x29);  // display on
  uiSetBrightness(cfg.brightness);
}

void uiMessage(const char *m, uint32_t ms) {
  strncpy(msg, m, sizeof(msg) - 1);
  msg[sizeof(msg) - 1] = 0;
  msgUntil = millis() + ms;
}

bool uiInGame() { return scr == S_GAME; }
void uiGoHome() { scr = S_PET; }

// The panel has rounded corners: keep content inside these margins.
#define LM 22

static void line(int y, uint16_t col, const char *fmt, ...) {
  char b[40];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(b, sizeof(b), fmt, ap);
  va_end(ap);
  cv->setTextColor(col);
  cv->setCursor(LM, y);
  cv->print(b);
}

void uiBootScreen() {
  cv->fillScreen(C565(10, 12, 30));
  cv->setTextSize(2);
  line(26, C565(255, 255, 255), "Pocket Pet");
  cv->setTextSize(1);
  struct Row { const char *n; bool ok; bool ext; } rows[] = {
    {"Touch CST816T 0x15", i2cProbe(ADDR_TOUCH), false},
    {"RTC PCF85063 0x51", i2cProbe(ADDR_RTC), false},
    {"AHT10 temp/hum 0x38", env.hasTH, true},
    {"VEML7700 light 0x10", env.hasLux, true},
    {"DRV2605L haptic 0x5A", hapticsPresent(), true},
  };
  int y = 56;
  for (auto &r : rows) {
    line(y, r.ok ? C565(90, 255, 120) : (r.ext ? C565(255, 200, 80) : C565(255, 90, 90)), "%s: %s", r.n, r.ok ? "OK" : (r.ext ? "not attached" : "MISSING"));
    y += 16;
  }
  line(y + 8, C565(180, 180, 200), "External boards are re-scanned");
  line(y + 22, C565(180, 180, 200), "every 10 s: plug in any time.");
  line(y + 44, C565(180, 180, 200), "Battery %.2f V", batteryVolts());
  cv->flush();
}

static bool dbgTempIsOff() {
#if POCKET_PET_DEBUG
  return isnan(dbgTempC);
#else
  return true;
#endif
}

// ---------------- pet screen ----------------
struct Btn { int x, y, w, h; const char *t; };
static const Btn BTN_FEED = {26, 240, 62, 32, "Feed"};
static const Btn BTN_PLAY = {92, 240, 62, 32, "Play"};
static const Btn BTN_MENU = {158, 240, 62, 32, "Menu"};
static bool hit(const Btn &b, int x, int y) { return x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h; }
static void drawBtn(const Btn &b, uint16_t col) {
  cv->fillRoundRect(b.x, b.y, b.w, b.h, 8, col);
  cv->setTextColor(C565(255, 255, 255));
  cv->setTextSize(2);
  cv->setCursor(b.x + (b.w - 12 * strlen(b.t)) / 2, b.y + 9);
  cv->print(b.t);
}

static void bar(int y, const char *label, float v, uint16_t col) {
  cv->setTextSize(1);
  cv->setTextColor(C565(220, 220, 230));
  cv->setCursor(LM + 2, y + 2);
  cv->print(label);
  cv->drawRect(70, y, 148, 10, C565(200, 200, 210));
  cv->fillRect(72, y + 2, (int)(144 * v / 100.0f), 6, col);
}

static void tempEffect(float t, bool known, uint32_t ms) {
  if (!known) return;
  if (t < 16) {  // cold: falling snow
    for (int i = 0; i < 14; i++) {
      int x = (i * 37 + 11) % 236;
      int y = 80 + (int)((ms / 20 + i * 53) % 150);
      cv->fillRect(x, y, 3, 3, C565(235, 245, 255));
    }
  } else if (t > 28) {  // hot: heat shimmer + sweat drops
    for (int i = 0; i < 5; i++) {
      int y = 90 + i * 28;
      for (int x = 0; x < 240; x += 6) cv->drawPixel(x, y + (int)(3 * sin((x + ms / 8) * 0.12f + i)), C565(255, 150, 90));
    }
    cv->fillCircle(178, 130 + (ms / 40) % 20, 3, C565(120, 200, 255));
  }
}

static void drawPet(uint32_t now, uint32_t nowEpoch) {
  float t = tempNow();
  bool known = sensorsTempKnown();
  uint16_t bg = C565(18, 30, 44);
  if (known && t < 16) bg = C565(14, 30, 70);
  else if (known && t > 28) bg = C565(70, 28, 18);
  cv->fillScreen(bg);

  int h, m, yr, mo, dy, wd;
  timeParts(nowEpoch, h, m);
  dateParts(nowEpoch, yr, mo, dy, wd);
  static const char *WD[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char *MO[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char b1[40], b2[48];
  snprintf(b1, sizeof(b1), "%s %d %s %d  %02d:%02d", WD[wd], dy, MO[mo - 1], yr, h, m);
  int n = 0;
  if (sensorsTempKnown()) n += snprintf(b2 + n, sizeof(b2) - n, "%.1fC ", t);
  if (sensorsTempKnown() && !isnan(env.hum) && dbgTempIsOff()) n += snprintf(b2 + n, sizeof(b2) - n, "%.0f%% ", env.hum);
  else n += snprintf(b2 + n, sizeof(b2) - n, "%s ", cfg.tempSensor ? "no temp" : "temp off");
  if (cfg.lightSensor && env.hasLux) n += snprintf(b2 + n, sizeof(b2) - n, "%.0flx ", env.lux);
  snprintf(b2 + n, sizeof(b2) - n, "%.2fV", batteryVolts());
  cv->setTextSize(1);
  cv->setTextColor(C565(255, 255, 255));
  cv->setCursor(120 - 3 * strlen(b1), 14);
  cv->print(b1);
  cv->setTextColor(C565(190, 200, 215));
  cv->setCursor(120 - 3 * strlen(b2), 28);
  cv->print(b2);

  if (petIsEgg(nowEpoch)) {
    uint32_t left = pet.hatchEpoch - nowEpoch;
    cv->setTextSize(1);
    cv->setTextColor(C565(255, 255, 255));
    char hb[32];
    snprintf(hb, sizeof(hb), "Something is hatching... %lus", (unsigned long)left);
    cv->setCursor(120 - 3 * strlen(hb), 54);
    cv->print(hb);
    cv->drawRect(40, 68, 160, 8, C565(200, 200, 210));
    cv->fillRect(42, 70, (int)(156 * petHatchProgress(nowEpoch)), 4, C565(255, 225, 110));
  } else {
  bar(44, "Food", pet.fullness, C565(255, 170, 60));
  bar(58, "Fun", pet.happiness, C565(255, 120, 200));
  bar(72, "Health", pet.health, C565(90, 230, 120));
  }

  Mood md = petMood();
  bool blink = (now / 200) % 20 == 0;
  int bob = (int)(4 * sin(now / 350.0f));
  if (md == MOOD_DEAD) bob = 0;
  bool egg = petIsEgg(nowEpoch);
  if (egg) spriteDrawEgg(cv, (240 - SPRITE_PX) / 2, 90, petHatchProgress(nowEpoch), now);
  else spriteDraw(cv, (240 - SPRITE_PX) / 2, 90, md, blink, bob);
  tempEffect(t, known, now);

  cv->setTextSize(2);
  cv->setTextColor(C565(255, 255, 255));
  if (now < msgUntil) {
    cv->setCursor(120 - 6 * strlen(msg), 220);
    cv->print(msg);
  } else if (md == MOOD_DEAD) {
    cv->setCursor(60, 220);
    cv->print("Tap: new egg");
  }
  bool eggNow = petIsEgg(nowEpoch);
  drawBtn(BTN_FEED, eggNow ? C565(60, 60, 70) : C565(200, 120, 30));
  drawBtn(BTN_PLAY, eggNow ? C565(60, 60, 70) : C565(190, 60, 140));
  drawBtn(BTN_MENU, C565(60, 80, 160));
}

// ---------------- settings screen ----------------
#if POCKET_PET_DEBUG
#define NROWS 10
#define DBG_ROW 8
#define BACK_ROW 9
#else
#define NROWS 9
#define BACK_ROW 8
#endif
#define ROW_Y0 18
#define ROW_STEP 25
static const uint8_t BRIGHT_STEPS[] = {30, 80, 130, 180, 255};

static void rowText(int i, char *out) {
  switch (i) {
    case 0: sprintf(out, "Temp: %s%s", cfg.tempSensor ? "ON" : "OFF", env.hasTH ? "" : " (none)"); break;
    case 1: sprintf(out, "Light: %s%s", cfg.lightSensor ? "ON" : "OFF", env.hasLux ? "" : " (none)"); break;
    case 2: if (cfg.volume) sprintf(out, "Sound: vol %d", cfg.volume); else sprintf(out, "Sound: MUTE"); break;
    case 3: sprintf(out, "Haptics: %s%s", cfg.haptics ? "ON" : "OFF", hapticsPresent() ? "" : " n/a"); break;
    case 4: if (cfg.autoBright) sprintf(out, "Auto bright: ON"); else sprintf(out, "Auto: %s", lightUsable() ? "OFF" : "OFF (no lux)"); break;
    case 5:
      if (cfg.autoBright) sprintf(out, "Bright: auto");
      else sprintf(out, "Bright: %d%%", (int)(cfg.brightness * 100 / 255));
      break;
    case 6: sprintf(out, "Sleep poll: %dm", cfg.sleepPollS / 60); break;
    case 7: sprintf(out, "Quiet hrs: %s", cfg.quiet ? "ON" : "OFF"); break;
    #if POCKET_PET_DEBUG
    case DBG_ROW: strcpy(out, "Debug menu"); break;
#endif
    default: strcpy(out, "Back");
  }
}

static void drawSettings() {
  cv->fillScreen(C565(16, 18, 30));
  cv->setTextSize(2);
  for (int i = 0; i < NROWS; i++) {
    int y = ROW_Y0 + i * ROW_STEP;
    bool dim = (i == 5 && cfg.autoBright) || (i == 4 && !cfg.autoBright && !lightUsable());
    cv->fillRoundRect(16, y, 208, 23, 6, i == BACK_ROW ? C565(60, 80, 160) : (dim ? C565(28, 30, 46) : C565(40, 44, 70)));
    char b[32];
    rowText(i, b);
    cv->setTextColor(dim ? C565(130, 130, 150) : C565(255, 255, 255));
    cv->setCursor(24, y + 4);
    cv->print(b);
  }
}

static void settingsTap(int y) {
  if (y < ROW_Y0) return;
  int i = (y - ROW_Y0) / ROW_STEP;
  switch (i) {
    case 0: cfg.tempSensor = !cfg.tempSensor; break;
    case 1:
      cfg.lightSensor = !cfg.lightSensor;
      if (cfg.autoBright && !lightUsable()) {
        cfg.autoBright = false;
        uiSetBrightness(cfg.brightness);
      }
      break;
    case 2: cfg.volume = (cfg.volume + 1) % 4; soundPlay(EM_TAP); break;
    case 3: cfg.haptics = !cfg.haptics; hapticsPlay(HX_TAP); break;
    case 4:
      if (!cfg.autoBright && !lightUsable()) {
        uiMessage(cfg.lightSensor ? "No light sensor" : "Turn Light ON", 1500);
        break;
      }
      cfg.autoBright = !cfg.autoBright;
      if (!cfg.autoBright) uiSetBrightness(cfg.brightness);
      break;
    case 5: {  // manual brightness only applies with auto brightness off
      if (cfg.autoBright) break;
      int k = 0;
      while (k < 5 && BRIGHT_STEPS[k] <= cfg.brightness) k++;
      cfg.brightness = BRIGHT_STEPS[k % 5];
      uiSetBrightness(cfg.brightness);
      break;
    }
    case 6: cfg.sleepPollS = cfg.sleepPollS == 60 ? 300 : cfg.sleepPollS == 300 ? 900 : cfg.sleepPollS == 900 ? 1800 : 60; break;
    case 7: cfg.quiet = !cfg.quiet; break;
#if POCKET_PET_DEBUG
    case DBG_ROW: scr = S_DEBUG; break;
#endif
    case BACK_ROW: settingsSave(); scr = S_PET; break;
  }
}

#if POCKET_PET_DEBUG
// ---------------- debug screen: left half of a row = decrease/first action, right half = increase/second ----------------
static const float TEMP_PRESETS[] = {NAN, 0, 10, 15, 22, 30, 36};
static uint8_t tempIdx = 0, emoteIdx = 0, hapIdx = 0;
#define DBG_ROWS 8

static float *statPtr(int i) { return i == 0 ? &pet.fullness : i == 1 ? &pet.happiness : &pet.health; }

static void dbgRowText(int i, char *out) {
  switch (i) {
    case 0: case 1: case 2: sprintf(out, "%s %d", i == 0 ? "Food" : i == 1 ? "Fun" : "Health", (int)*statPtr(i)); break;
    case 3: strcpy(out, "Warp +1h | +6h"); break;
    case 4: if (isnan(dbgTempC)) strcpy(out, "Temp: sensor"); else sprintf(out, "Temp: %.0fC", dbgTempC); break;
    case 5: sprintf(out, "Kill | Revive"); break;
    case 6: sprintf(out, "Snd %d | Hap %d", emoteIdx, hapIdx); break;
    default: strcpy(out, "Back");
  }
}

static void drawDebug() {
  cv->fillScreen(C565(40, 10, 20));
  cv->setTextSize(1);
  cv->setTextColor(C565(255, 120, 120));
  cv->setCursor(88, 8);
  cv->print("DEBUG (test only)");
  cv->setTextSize(2);
  for (int i = 0; i < DBG_ROWS; i++) {
    int y = ROW_Y0 + 8 + i * ROW_STEP;
    cv->fillRoundRect(16, y, 208, 23, 6, i == DBG_ROWS - 1 ? C565(60, 80, 160) : C565(70, 30, 44));
    char b[32];
    dbgRowText(i, b);
    cv->setTextColor(C565(255, 255, 255));
    if (i <= 2) {  // - / + markers for the stat rows
      cv->setCursor(22, y + 4); cv->print("-");
      cv->setCursor(206, y + 4); cv->print("+");
    }
    cv->setCursor(i <= 2 ? 44 : 24, y + 4);
    cv->print(b);
  }
}

static void debugTap(int x, int y) {
  if (y < ROW_Y0 + 8) return;
  int i = (y - (ROW_Y0 + 8)) / ROW_STEP;
  bool right = x >= 120;
  switch (i) {
    case 0: case 1: case 2: {
      float *p = statPtr(i);
      *p = constrain(*p + (right ? 10 : -10), 0.0f, 100.0f);
      if (i == 2 && *p > 0) pet.alive = 1;
      if (i == 2 && *p <= 0) pet.alive = 0;
      petSave();
      break;
    }
    case 3: petDebugWarp(right ? 21600 : 3600); petSave(); break;
    case 4: {
      tempIdx = (tempIdx + 1) % (sizeof(TEMP_PRESETS) / sizeof(TEMP_PRESETS[0]));
      dbgTempC = TEMP_PRESETS[tempIdx];
      break;
    }
    case 5:
      if (right) { pet.alive = 1; if (pet.health < 50) pet.health = 50; if (pet.fullness < 50) pet.fullness = 50; if (pet.happiness < 50) pet.happiness = 50; }
      else { pet.health = 0; pet.alive = 0; soundPlay(EM_DEAD); }
      petSave();
      break;
    case 6:
      if (right) { hapticsPlay((HapticFx)hapIdx); hapIdx = (hapIdx + 1) % 4; }
      else { soundPlay((Emote)emoteIdx); emoteIdx = (emoteIdx + 1) % 9; }
      break;
    case 7: scr = S_SETTINGS; break;
  }
}
#endif

// ---------------- dispatch ----------------
void uiTap(int x, int y) {
  uint32_t now = millis();
  if (now - lastTapMs < 150) return;
  lastTapMs = now;
#if POCKET_PET_DEBUG
  if (scr == S_DEBUG) { debugTap(x, y); return; }
#endif
  if (scr == S_SETTINGS) { settingsTap(y); return; }
  if (scr == S_GAME) {
    if (gameFinished()) {
      petPlay(gameScore());
      petSave();
      soundPlay(EM_HAPPY);
      uiMessage("That was fun!");
      scr = S_PET;
    } else if (gameTap(x, y)) {
      soundPlay(EM_TAP);
      hapticsPlay(HX_TAP);
    }
    return;
  }
  if (petIsEgg(rtcNow())) {  // hatching: only the Menu button and poking the egg work
    if (hit(BTN_MENU, x, y)) scr = S_SETTINGS;
    else if (y > 84 && y < 218) { hapticsPlay(HX_TAP); soundPlay(EM_TAP); }
    return;
  }
  if (!pet.alive) {
    if (y < 238) { petReset(rtcNow()); soundPlay(EM_HAPPY); hapticsPlay(HX_DOUBLE); uiMessage("A new egg hatched!"); }
    return;
  }
  if (hit(BTN_FEED, x, y)) {
    if (petFeed()) { soundPlay(EM_EAT); hapticsPlay(HX_TAP); uiMessage("Yum!"); petSave(); }
    else { soundPlay(EM_SAD); uiMessage("Not hungry"); }
  } else if (hit(BTN_PLAY, x, y)) {
    gameStart();
    scr = S_GAME;
    soundPlay(EM_PLAY);
  } else if (hit(BTN_MENU, x, y)) {
    scr = S_SETTINGS;
  } else if (y > 84 && y < 218) {  // pet it
    pet.happiness = min(100.0f, pet.happiness + 1.0f);
    soundPlay(EM_HAPPY);
    hapticsPlay(HX_TAP);
  }
}

void uiDraw(uint32_t nowEpoch) {
  uint32_t now = millis();
  if (scr == S_GAME) {
    gameUpdate(now);
    gameDraw(cv);
#if POCKET_PET_DEBUG
  } else if (scr == S_DEBUG) {
    drawDebug();
#endif
  } else if (scr == S_SETTINGS) {
    drawSettings();
  } else {
    drawPet(now, nowEpoch);
  }
  cv->flush();
}
