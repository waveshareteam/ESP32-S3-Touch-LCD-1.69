#include "board.h"
#include <Wire.h>
#include <string.h>

bool i2cProbe(uint8_t a) {
  Wire.beginTransmission(a);
  return Wire.endTransmission() == 0;
}
bool i2cWriteRaw(uint8_t a, const uint8_t *d, size_t n) {
  Wire.beginTransmission(a);
  Wire.write(d, n);
  return Wire.endTransmission() == 0;
}
bool i2cReadRaw(uint8_t a, uint8_t *d, size_t n) {
  if (Wire.requestFrom((int)a, (int)n) != (int)n) return false;
  for (size_t i = 0; i < n; i++) d[i] = Wire.read();
  return true;
}
bool i2cWriteReg(uint8_t a, uint8_t reg, const uint8_t *d, size_t n) {
  Wire.beginTransmission(a);
  Wire.write(reg);
  Wire.write(d, n);
  return Wire.endTransmission() == 0;
}
bool i2cWrite8(uint8_t a, uint8_t reg, uint8_t v) { return i2cWriteReg(a, reg, &v, 1); }
bool i2cReadReg(uint8_t a, uint8_t reg, uint8_t *d, size_t n) {
  Wire.beginTransmission(a);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  return i2cReadRaw(a, d, n);
}

// ---- touch (CST816T): reg 0x02 = finger count, 0x03..0x06 = x/y ----
bool touchGet(int &x, int &y) {
  uint8_t b[5];
  if (!i2cReadReg(ADDR_TOUCH, 0x02, b, 5)) return false;
  if ((b[0] & 0x0F) == 0) return false;
  x = ((b[1] & 0x0F) << 8) | b[2];
  y = ((b[3] & 0x0F) << 8) | b[4];
  return true;
}

// ---- RTC (PCF85063) ----
static uint32_t toEpoch(int y, int m, int d, int hh, int mm, int ss) {
  y -= m <= 2;
  int era = y / 400;
  unsigned yoe = y - era * 400;
  unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long days = (long)era * 146097 + doe - 719468;
  return (uint32_t)(days * 86400L + hh * 3600L + mm * 60L + ss);
}
static void fromEpoch(uint32_t t, int &y, int &m, int &d, int &hh, int &mm, int &ss) {
  long days = t / 86400;
  long rem = t % 86400;
  hh = rem / 3600;
  mm = (rem % 3600) / 60;
  ss = rem % 60;
  long z = days + 719468;
  long era = z / 146097;
  unsigned doe = z - era * 146097;
  unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  y = yoe + era * 400;
  unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  unsigned mp = (5 * doy + 2) / 153;
  d = doy - (153 * mp + 2) / 5 + 1;
  m = mp < 10 ? mp + 3 : mp - 9;
  y += m <= 2;
}
static uint8_t bcd2(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
static uint8_t toBcd(int v) { return ((v / 10) << 4) | (v % 10); }

static uint32_t buildEpoch() {
  static const char *mon = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char mstr[4] = {__DATE__[0], __DATE__[1], __DATE__[2], 0};
  int m = (strstr(mon, mstr) - mon) / 3 + 1;
  int d = atoi(__DATE__ + 4);
  int y = atoi(__DATE__ + 7);
  return toEpoch(y, m, d, atoi(__TIME__), atoi(__TIME__ + 3), atoi(__TIME__ + 6));
}

void rtcSet(uint32_t e) {
  int y, m, d, hh, mm, ss;
  fromEpoch(e, y, m, d, hh, mm, ss);
  uint8_t b[7] = {toBcd(ss), toBcd(mm), toBcd(hh), toBcd(d), 0, toBcd(m), toBcd(y - 2000)};
  i2cWriteReg(ADDR_RTC, 0x04, b, 7);  // writing seconds clears the OS (oscillator-stop) flag
}

uint32_t rtcNow() {
  uint8_t b[7];
  if (!i2cReadReg(ADDR_RTC, 0x04, b, 7)) return buildEpoch();
  return toEpoch(2000 + bcd2(b[6]), bcd2(b[5] & 0x1F), bcd2(b[3] & 0x3F), bcd2(b[2] & 0x3F), bcd2(b[1] & 0x7F), bcd2(b[0] & 0x7F));
}

static void rtcInit() {
  uint8_t sec = 0x80;
  i2cReadReg(ADDR_RTC, 0x04, &sec, 1);
  // Oscillator-stop flag set (first boot / power was lost) or clock behind the build time: seed from build time.
  if ((sec & 0x80) || rtcNow() < buildEpoch()) rtcSet(buildEpoch());
}

int hourOfDay(uint32_t e) { return (e % 86400) / 3600; }
void dateParts(uint32_t e, int &y, int &m, int &d, int &wd) {
  int hh, mm, ss;
  fromEpoch(e, y, m, d, hh, mm, ss);
  wd = (e / 86400 + 4) % 7;  // 1970-01-01 was a Thursday
}
void timeParts(uint32_t e, int &h, int &m) {
  h = (e % 86400) / 3600;
  m = (e % 3600) / 60;
}

float batteryVolts() { return analogReadMilliVolts(PIN_BAT_ADC) * 3.0f / 1000.0f; }  // 200k/100k divider

void powerLatchOff() { digitalWrite(PIN_SYS_EN, LOW); }

void boardInit() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_SYS_EN, OUTPUT);
  digitalWrite(PIN_SYS_EN, HIGH);
  pinMode(PIN_SYS_OUT, INPUT);
  Wire.begin(IIC_SDA, IIC_SCL);
  Wire.setClock(400000);
  pinMode(TP_RST, OUTPUT);
  digitalWrite(TP_RST, LOW);
  delay(10);
  digitalWrite(TP_RST, HIGH);
  delay(60);
  pinMode(TP_INT, INPUT);
  rtcInit();
}
