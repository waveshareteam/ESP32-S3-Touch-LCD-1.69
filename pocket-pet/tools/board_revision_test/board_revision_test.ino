// Board revision test for ESP32-S3-Touch-LCD-1.69 (V1 vs V2.1).
//
// Read-only detection: the PCF85063 RTC interrupt line is wired to GPIO41 on V1
// and to GPIO39 on V2.1. The sketch arms a 2 s RTC countdown timer, watches both
// pins (as inputs only, never outputs), and reports which one is pulled low.
// It never drives GPIO33/35/36/40/41/42, never sleeps and never touches GPIO0,
// so the board can always be re-flashed normally.

#include <Arduino.h>
#include <Wire.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

#if ARDUINO_USB_CDC_ON_BOOT
#define OUT Serial
#else
#include "HWCDC.h"
HWCDC USBSerial;
#define OUT USBSerial
#endif

static const uint8_t RTC_ADDR = 0x51;
static const int PIN_RTC_INT_V21 = 39;
static const int PIN_RTC_INT_V1 = 41;  // SYS_EN on V2.1: input only, never output

// PCF85063 registers
static const uint8_t REG_CTRL2 = 0x01;
static const uint8_t REG_TIMER_VALUE = 0x10;
static const uint8_t REG_TIMER_MODE = 0x11;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 20);

struct Dev {
  uint8_t addr;
  const char *name;
  bool onboard;
  bool found;
};

static Dev devs[] = {
  { 0x15, "CST816T touch", true, false },
  { 0x51, "PCF85063 RTC", true, false },
  { 0x6B, "QMI8658 IMU", true, false },
  { 0x10, "VEML7700 light", false, false },
  { 0x38, "AHT10 temp/hum", false, false },
  { 0x5A, "DRV2605L haptic", false, false },
};

static bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

static bool rtcRead(uint8_t reg, uint8_t &val) {
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)RTC_ADDR, 1) != 1) return false;
  val = Wire.read();
  return true;
}

static bool rtcWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static int line = 0;
static void say(uint16_t color, const char *fmt, ...) {
  char buf[96];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  OUT.println(buf);
  gfx->setTextColor(color);
  gfx->setCursor(4, 4 + line * 12);
  gfx->print(buf);
  line++;
}

void setup() {
  OUT.begin(115200);

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  if (!gfx->begin()) OUT.println("gfx->begin() failed!");
  gfx->fillScreen(RGB565_BLACK);
  gfx->setTextSize(1);

  Wire.begin(IIC_SDA, IIC_SCL);
  Wire.setClock(100000);

  // Wake the touch controller so it answers the scan (reset pulse only).
  pinMode(TP_RST, OUTPUT);
  digitalWrite(TP_RST, LOW);
  delay(10);
  digitalWrite(TP_RST, HIGH);
  delay(60);

  say(RGB565_WHITE, "Board revision test");
  say(RGB565_CYAN, "I2C scan (SDA%d SCL%d):", IIC_SDA, IIC_SCL);
  for (auto &d : devs) {
    d.found = i2cPresent(d.addr);
    if (d.onboard || d.found)
      say(d.found ? RGB565_GREEN : RGB565_RED, " 0x%02X %s: %s", d.addr, d.name, d.found ? "OK" : "missing");
  }

  bool rtcOk = devs[1].found;
  int low39 = 0, low41 = 0, base39 = -1, base41 = -1;
  bool tf = false;

  if (rtcOk) {
    // Inputs with pull-up: the RTC INT output is open-drain, active low.
    pinMode(PIN_RTC_INT_V21, INPUT_PULLUP);
    pinMode(PIN_RTC_INT_V1, INPUT_PULLUP);
    delay(20);
    base39 = digitalRead(PIN_RTC_INT_V21);
    base41 = digitalRead(PIN_RTC_INT_V1);

    uint8_t c2 = 0, tm = 0;
    rtcRead(REG_CTRL2, c2);
    rtcRead(REG_TIMER_MODE, tm);
    rtcWrite(REG_CTRL2, c2 & ~0x08);   // clear TF
    rtcWrite(REG_TIMER_VALUE, 2);      // 2 ticks
    rtcWrite(REG_TIMER_MODE, 0x16);    // 1 Hz, timer enabled, interrupt enabled, INT follows TF
    uint32_t t0 = millis();
    while (millis() - t0 < 4000) {
      if (digitalRead(PIN_RTC_INT_V21) == LOW) low39++;
      if (digitalRead(PIN_RTC_INT_V1) == LOW) low41++;
      uint8_t s;
      if (rtcRead(REG_CTRL2, s) && (s & 0x08)) tf = true;
      delay(20);
    }
    // Restore: stop timer, clear flag, put pins back to plain inputs.
    rtcWrite(REG_TIMER_MODE, tm);
    rtcRead(REG_CTRL2, c2);
    rtcWrite(REG_CTRL2, c2 & ~0x08);
    pinMode(PIN_RTC_INT_V21, INPUT);
    pinMode(PIN_RTC_INT_V1, INPUT);
  }

  gfx->drawFastHLine(0, 4 + line * 12, gfx->width(), RGB565_DARKGREY);
  line++;
  say(RGB565_CYAN, "RTC INT probe (idle/low-count):");
  if (rtcOk) {
    say(RGB565_WHITE, " GPIO39: idle=%d low=%d", base39, low39);
    say(RGB565_WHITE, " GPIO41: idle=%d low=%d", base41, low41);
    say(RGB565_WHITE, " RTC timer flag fired: %s", tf ? "yes" : "no");
  } else {
    say(RGB565_RED, " RTC not found, no probe");
  }

  const char *rev = "INCONCLUSIVE";
  const char *conf = "none";
  uint16_t col = RGB565_YELLOW;
  bool hit39 = rtcOk && tf && low39 > 0 && base39 == 1;
  bool hit41 = rtcOk && tf && low41 > 0 && base41 == 1;
  if (hit39 && !hit41) { rev = "V2.1"; conf = "high"; col = RGB565_GREEN; }
  else if (hit41 && !hit39) { rev = "V1"; conf = "high"; col = RGB565_GREEN; }

  line++;
  gfx->setTextSize(2);
  gfx->setCursor(4, 4 + line * 12);
  gfx->setTextColor(col);
  gfx->printf("Rev: %s", rev);
  OUT.printf("Rev: %s (confidence: %s)\n", rev, conf);
  gfx->setTextSize(1);
  line += 2;
  say(col, "Confidence: %s", conf);

  if (!strcmp(rev, "V2.1")) {
    say(RGB565_WHITE, "Buzzer 42, SYS_EN 41,");
    say(RGB565_WHITE, "SYS_OUT 40, RTC_INT 39");
  } else if (!strcmp(rev, "V1")) {
    say(RGB565_WHITE, "Buzzer 33, SYS_EN 35,");
    say(RGB565_WHITE, "SYS_OUT 36, RTC_INT 41");
  } else {
    say(RGB565_YELLOW, "Check: RTC at 0x51 present?");
    say(RGB565_YELLOW, "USB powered (not battery)?");
    say(RGB565_YELLOW, "Re-run; look at PCB marks;");
    say(RGB565_YELLOW, "try example 09 pin toggle.");
  }
  say(RGB565_DARKGREY, "No sleep: reflash anytime.");
}

void loop() {
  delay(1000);
}
