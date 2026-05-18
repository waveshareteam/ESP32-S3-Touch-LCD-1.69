#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "SensorPCF85063.hpp"
#include <Wire.h>

#include "HWCDC.h"
HWCDC USBSerial;

SensorPCF85063 rtc;
uint32_t lastMillis;
char previousTimeString[20] = "";
Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);

Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
                                      0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);

const uint8_t TEXT_SIZE = 3;
const int16_t FONT_WIDTH = 6;
const int16_t FONT_HEIGHT = 8;
const int16_t LINE_GAP = 10;

int16_t getCenteredX(const char *text, uint8_t textSize) {
  int16_t textWidth = strlen(text) * FONT_WIDTH * textSize;
  int16_t x = (LCD_WIDTH - textWidth) / 2;
  return x > 0 ? x : 0;
}

int16_t getTextBlockY() {
  int16_t lineHeight = FONT_HEIGHT * TEXT_SIZE;
  int16_t totalHeight = (lineHeight * 2) + LINE_GAP;
  return (LCD_HEIGHT - totalHeight) / 2;
}

void drawCenteredText(const char *text, int16_t y) {
  gfx->setCursor(getCenteredX(text, TEXT_SIZE), y);
  gfx->println(text);
}

void setup() {
  USBSerial.begin(115200);
  if (!rtc.begin(Wire, IIC_SDA, IIC_SCL)) {
    USBSerial.println("Failed to find PCF85063 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }

  uint16_t year = 2024;
  uint8_t month = 9;
  uint8_t day = 24;
  uint8_t hour = 11;
  uint8_t minute = 24;
  uint8_t second = 30;

  rtc.setDateTime(year, month, day, hour, minute, second);
  gfx->begin();
  gfx->fillScreen(RGB565_WHITE);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
}

void loop() {

  if (millis() - lastMillis > 1000) {
    lastMillis = millis();

    RTC_DateTime datetime = rtc.getDateTime();

    char dateString[11];
    char timeString[9];
    char dateTimeString[20];

    sprintf(dateString, "%04d-%02d-%02d", datetime.getYear(), datetime.getMonth(), datetime.getDay());
    sprintf(timeString, "%02d:%02d:%02d", datetime.getHour(), datetime.getMinute(), datetime.getSecond());
    sprintf(dateTimeString, "%s %s", dateString, timeString);

    // Only update the time if it has changed
    if (strcmp(dateTimeString, previousTimeString) != 0) {
      int16_t lineHeight = FONT_HEIGHT * TEXT_SIZE;
      int16_t dateY = getTextBlockY();
      int16_t timeY = dateY + lineHeight + LINE_GAP;

      gfx->fillRect(0, dateY - 4, LCD_WIDTH, (lineHeight * 2) + LINE_GAP + 8, RGB565_WHITE);
      gfx->setTextColor(RGB565_BLACK);
      gfx->setTextSize(TEXT_SIZE);

      drawCenteredText(dateString, dateY);
      drawCenteredText(timeString, timeY);

      strcpy(previousTimeString, dateTimeString);
    }
  }
}
