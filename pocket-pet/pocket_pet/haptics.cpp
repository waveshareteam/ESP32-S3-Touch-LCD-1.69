#include "haptics.h"
#include "board.h"
#include "settings.h"
#include <Preferences.h>

// Motor parameters for an 8x3.2 mm LRA: TUNE FROM YOUR MOTOR DATASHEET.
#define LRA_RATED_V_REG 0x53   // ~1.8 Vrms (see DRV2605L datasheet formula)
#define LRA_OD_CLAMP_REG 0x60  // ~2.1 V peak
#define LRA_DRIVE_TIME 20      // half-period of ~200 Hz resonance: (2.5 ms - 0.5 ms)/0.1 ms

static bool present = false;

static uint8_t rd(uint8_t r) {
  uint8_t v = 0;
  i2cReadReg(ADDR_DRV, r, &v, 1);
  return v;
}

static void configure() {
  i2cWrite8(ADDR_DRV, 0x01, 0x00);  // out of standby, internal trigger
  i2cWrite8(ADDR_DRV, 0x03, 6);     // LRA library (library 6)
  i2cWrite8(ADDR_DRV, 0x16, LRA_RATED_V_REG);
  i2cWrite8(ADDR_DRV, 0x17, LRA_OD_CLAMP_REG);
  i2cWrite8(ADDR_DRV, 0x1A, 0xBA);  // N_ERM_LRA=1 (LRA), brake factor 3, loop gain 2, BEMF gain 2
  i2cWrite8(ADDR_DRV, 0x1B, 0x80 | LRA_DRIVE_TIME);  // startup boost + drive time
  i2cWrite8(ADDR_DRV, 0x1E, 0x30);  // auto-cal time 1000-1200 ms
}

static bool autoCalibrate() {
  i2cWrite8(ADDR_DRV, 0x01, 0x07);  // auto-calibration mode
  i2cWrite8(ADDR_DRV, 0x0C, 0x01);  // GO
  uint32_t t0 = millis();
  while ((rd(0x0C) & 1) && millis() - t0 < 2500) delay(20);
  bool ok = !(rd(0x00) & 0x08);  // DIAG_RESULT=1 means calibration failed
  i2cWrite8(ADDR_DRV, 0x01, 0x00);
  return ok;
}

bool hapticsRescan() {
  if (present) return false;
  if (!i2cProbe(ADDR_DRV)) return false;
  present = true;
  i2cWrite8(ADDR_DRV, 0x01, 0x80);  // device reset
  delay(5);
  configure();
  Preferences p;
  p.begin("hap", false);
  uint8_t cal[3];
  if (p.getBytesLength("cal") == 3) {
    p.getBytes("cal", cal, 3);
    i2cWrite8(ADDR_DRV, 0x18, cal[0]);
    i2cWrite8(ADDR_DRV, 0x19, cal[1]);
    i2cWrite8(ADDR_DRV, 0x1A, cal[2]);
  } else if (autoCalibrate()) {
    cal[0] = rd(0x18);
    cal[1] = rd(0x19);
    cal[2] = rd(0x1A);
    p.putBytes("cal", cal, 3);
  }
  p.end();
  i2cWrite8(ADDR_DRV, 0x03, 6);
  return true;
}

void hapticsInit() { hapticsRescan(); }

void hapticsPlay(HapticFx fx) {
  if (!present || !cfg.haptics) return;
  static const uint8_t ids[] = {1, 10, 14, 15};  // strong click, double click, strong buzz, 750 ms alert
  i2cWrite8(ADDR_DRV, 0x01, 0x00);
  i2cWrite8(ADDR_DRV, 0x04, ids[fx]);
  i2cWrite8(ADDR_DRV, 0x05, 0);
  i2cWrite8(ADDR_DRV, 0x0C, 1);
}

void hapticsSleep() {
  if (present) i2cWrite8(ADDR_DRV, 0x01, 0x40);  // standby
}
void hapticsWake() {
  if (present) i2cWrite8(ADDR_DRV, 0x01, 0x00);
}
bool hapticsPresent() { return present; }
