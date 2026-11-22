#include "power.h"
#include <esp_sleep.h>
#include <driver/gpio.h>
#include "board.h"
#include "haptics.h"
#include "pet.h"
#include "sensors.h"
#include "settings.h"
#include "sound.h"
#include "ui.h"

void lowBatteryGuard() {
  float v = batteryVolts();
  if (v > 2.0f && v < 3.3f) {  // the board has no hardware under-voltage cutoff
    petSave();
    soundOff();
    uiScreenOff();
    powerLatchOff();  // on battery this removes power; on USB the board stays up and charges
    delay(1000);
    esp_deep_sleep(3600ULL * 1000000ULL);
  }
}

static void periodicWork() {
  sensorsRescan();
  hapticsRescan();
  sensorsPoll();
  petTick(rtcNow(), true);  // half-rate drain while asleep
  petSave();
  lowBatteryGuard();
}

void powerSleepUntilTouch() {
  petSave();
  soundOff();
  hapticsSleep();
  sensorsShutdown();
  uiScreenOff();
  uint32_t t0 = millis();
  while (digitalRead(TP_INT) == LOW && millis() - t0 < 500) delay(10);  // don't wake instantly

  gpio_wakeup_enable((gpio_num_t)TP_INT, GPIO_INTR_LOW_LEVEL);
  gpio_wakeup_enable((gpio_num_t)0, GPIO_INTR_LOW_LEVEL);  // BOOT button, input only
  esp_sleep_enable_gpio_wakeup();
  while (true) {
    esp_sleep_enable_timer_wakeup((uint64_t)cfg.sleepPollS * 1000000ULL);
    LOG.flush();
    esp_light_sleep_start();
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) break;
    periodicWork();
  }
  gpio_wakeup_disable((gpio_num_t)TP_INT);
  gpio_wakeup_disable((gpio_num_t)0);
  uiScreenOn();
  hapticsWake();
  petTick(rtcNow(), true);
}
