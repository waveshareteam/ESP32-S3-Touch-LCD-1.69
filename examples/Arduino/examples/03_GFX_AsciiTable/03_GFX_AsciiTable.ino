#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <Wire.h>
#include "HWCDC.h"

HWCDC USBSerial;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);

Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
                                      0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);

static constexpr uint16_t BACKGROUND_COLOR = RGB565_BLACK;
static constexpr uint16_t HEADER_TEXT_COLOR = RGB565_GREEN;
static constexpr uint16_t INDEX_TEXT_COLOR = RGB565_BLUE;
static constexpr uint16_t ASCII_TEXT_COLOR = RGB565_WHITE;

void setup(void) {
  USBSerial.begin(115200);
  // USBSerial.setDebugOutput(true);
  // while(!USBSerial);
  USBSerial.println("Arduino_GFX AsciiTable example");

  int columnCount = LCD_WIDTH / 8;
  int rowCount = LCD_HEIGHT / 10;

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin()) {
    USBSerial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BACKGROUND_COLOR);

  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  gfx->setTextColor(HEADER_TEXT_COLOR);
  for (int x = 0; x < columnCount; x++) {
    gfx->setCursor(10 + x * 8, 2);
    gfx->print(x, 16);
  }
  gfx->setTextColor(INDEX_TEXT_COLOR);
  for (int y = 0; y < rowCount; y++) {
    gfx->setCursor(2, 12 + y * 10);
    gfx->print(y, 16);
  }

  char c = 0;
  for (int y = 0; y < rowCount; y++) {
    for (int x = 0; x < columnCount; x++) {
      gfx->drawChar(10 + x * 8, 12 + y * 10, c++, ASCII_TEXT_COLOR, BACKGROUND_COLOR);
    }
  }

  delay(5000);  // 5 seconds
}

void loop() {
}
