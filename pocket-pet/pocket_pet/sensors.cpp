#include "sensors.h"
#include "board.h"
#include "settings.h"

SensorData env;
static bool ahtInit = false;

static bool readAHT() {
  if (!i2cProbe(ADDR_AHT)) return false;
  if (!ahtInit) {
    const uint8_t init[3] = {0xE1, 0x08, 0x00};  // AHT10 init/calibrate
    i2cWriteRaw(ADDR_AHT, init, 3);
    delay(15);
    ahtInit = true;
  }
  const uint8_t trig[3] = {0xAC, 0x33, 0x00};
  if (!i2cWriteRaw(ADDR_AHT, trig, 3)) return false;
  delay(85);
  uint8_t d[6];
  if (!i2cReadRaw(ADDR_AHT, d, 6) || (d[0] & 0x80)) return false;
  uint32_t h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
  uint32_t t = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];
  env.hum = h * 100.0f / 1048576.0f;
  env.tempC = t * 200.0f / 1048576.0f - 50.0f;
  return true;
}

static void vemlConfig(uint16_t v) {
  uint8_t b[2] = {(uint8_t)(v & 0xFF), (uint8_t)(v >> 8)};
  i2cWriteReg(ADDR_VEML, 0x00, b, 2);
}

static bool readVEML() {
  if (!i2cProbe(ADDR_VEML)) return false;
  vemlConfig(0x0000);  // gain x1, 100 ms, powered on
  delay(125);
  uint8_t b[2];
  bool ok = i2cReadReg(ADDR_VEML, 0x04, b, 2);
  vemlConfig(0x0001);  // shutdown between reads
  if (!ok) return false;
  env.lux = (uint16_t)(b[0] | (b[1] << 8)) * 0.0576f;
  return true;
}

void sensorsInit() {
  env.hasTH = i2cProbe(ADDR_AHT);
  env.hasLux = i2cProbe(ADDR_VEML);
  if (env.hasLux) vemlConfig(0x0001);
}

bool sensorsRescan() {
  bool th = i2cProbe(ADDR_AHT), lux = i2cProbe(ADDR_VEML);
  bool changed = th != env.hasTH || lux != env.hasLux;
  env.hasTH = th;
  env.hasLux = lux;
  if (!th) ahtInit = false;
  if (lux) vemlConfig(0x0001);
  return changed;
}

void sensorsPoll() {
  if (cfg.tempSensor) env.hasTH = readAHT();
  if (cfg.lightSensor) env.hasLux = readVEML();
}

void sensorsShutdown() {
  if (i2cProbe(ADDR_VEML)) vemlConfig(0x0001);
}

#if POCKET_PET_DEBUG
float dbgTempC = NAN;
#endif

float tempNow() {
#if POCKET_PET_DEBUG
  if (!isnan(dbgTempC)) return dbgTempC;
#endif
  return env.tempC;
}

bool lightUsable() { return cfg.lightSensor && env.hasLux; }

bool sensorsTempKnown() {
#if POCKET_PET_DEBUG
  if (!isnan(dbgTempC)) return true;
#endif
  return cfg.tempSensor && env.hasTH && !isnan(env.tempC); }
