// Pocket Pet prototype for Waveshare ESP32-S3-Touch-LCD-1.69 (board V2.1).
// Arduino IDE sketch; only external library: "GFX Library for Arduino" 1.6.5. See README.md.

#include <Wire.h>
#include "board.h"
#include "config.h"
#include "haptics.h"
#include "pet.h"
#include "power.h"
#include "sensors.h"
#include "settings.h"
#include "sound.h"
#include "ui.h"

#if !ARDUINO_USB_CDC_ON_BOOT
HWCDC USBSerial;
#endif

static uint32_t lastDraw, lastTick, lastSave, lastPoll, lastRescan, lastActivity;
static uint32_t lastHungryAlert, lastSadAlert;
static bool prevDown = false;
static bool wasEgg = false;

static void autoBrightness() {
  uint8_t level = cfg.brightness;
  if (cfg.autoBright && !lightUsable()) {  // auto needs an enabled, detected light sensor
    cfg.autoBright = false;
    settingsSave();
  }
  if (cfg.autoBright && !isnan(env.lux)) {
    float l = env.lux < 1 ? 1 : env.lux;
    level = constrain((int)(40 + 50 * log10f(l)), 40, 255);  // ~40 in the dark, 255 in bright light
  }
  uiSetBrightness(level);
}

static void alerts(uint32_t now) {
  if (!pet.alive) return;
  if (pet.fullness < 25 && now - lastHungryAlert > 1800000UL) {
    lastHungryAlert = now;
    uiMessage("I'm hungry!");
    soundPlay(EM_HUNGRY);
    hapticsPlay(HX_ALERT);
  } else if (pet.happiness < 25 && now - lastSadAlert > 1800000UL) {
    lastSadAlert = now;
    uiMessage("Play with me...");
    soundPlay(EM_SAD);
    hapticsPlay(HX_DOUBLE);
  }
}

void setup() {
  boardInit();  // buzzer LOW, SYS_EN HIGH, Wire.begin(11,10) once, touch reset, RTC
  LOG.begin(115200);
  soundInit();
  settingsLoad();
  uiInit();
  sensorsInit();   // external boards may be absent: probe, never block
  hapticsInit();
  uiBootScreen();
  petLoad(rtcNow());  // offline catch-up
  delay(1800);
  sensorsPoll();
  autoBrightness();
  soundPlay(EM_BOOT);
  hapticsPlay(HX_DOUBLE);
  wasEgg = petIsEgg(rtcNow());
  lastActivity = lastTick = lastSave = lastPoll = lastRescan = millis();
  LOG.printf("TH:%d LUX:%d HAP:%d vbat=%.2f\n", env.hasTH, env.hasLux, hapticsPresent(), batteryVolts());
}

void loop() {
  uint32_t now = millis();
  int x, y;
  bool down = touchGet(x, y);
  if (down) {
    lastActivity = now;
    if (!prevDown) uiTap(x, y);
  }
  prevDown = down;
  soundUpdate();

  if (now - lastTick >= 1000) {
    lastTick = now;
    uint32_t t = rtcNow();
    petTick(t, false);
    bool egg = petIsEgg(t);
    if (egg && petHatchProgress(t) > 0.3f && t % 5 == 0) { soundPlay(EM_TAP); hapticsPlay(HX_TAP); }  // cracking
    if (wasEgg && !egg) {
      uiMessage("It hatched!", 4000);
      soundPlay(EM_HAPPY);
      hapticsPlay(HX_DOUBLE);
      lastHungryAlert = lastSadAlert = now;
      petSave();
    }
    wasEgg = egg;
    alerts(now);
  }
  if (now - lastSave >= 300000UL) {
    lastSave = now;
    petSave();
    lowBatteryGuard();
  }
  if (now - lastRescan >= 10000) {  // hot-plug: external boards may be attached after boot
    lastRescan = now;
    if (sensorsRescan()) uiMessage("Sensor board change", 2000);
    if (hapticsRescan()) { uiMessage("Haptics found", 2000); hapticsPlay(HX_DOUBLE); }
    autoBrightness();
  }
  if ((cfg.tempSensor || cfg.lightSensor) && now - lastPoll >= (uint32_t)cfg.awakePollS * 1000) {
    lastPoll = now;
    sensorsPoll();
    autoBrightness();
  }
  if (now - lastDraw >= 66) {
    lastDraw = now;
    uiDraw(rtcNow());
  }
  if (!uiInGame() && !petIsEgg(rtcNow()) && now - lastActivity > (uint32_t)cfg.screenTimeoutS * 1000) {
    powerSleepUntilTouch();
    autoBrightness();
    lastActivity = lastTick = lastPoll = millis();
    prevDown = true;  // swallow the waking touch
  }
  delay(5);
}
