#include <Wire.h>
#include <Arduino.h>
#include "pin_config.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "HWCDC.h"
HWCDC USBSerial;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);

Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
                                      0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);

static constexpr uint16_t DRAW_COLOR = RGB565_BLUE;
static constexpr uint16_t BACKGROUND_COLOR = RGB565_WHITE;
static constexpr int16_t DRAW_RADIUS = 3;
static constexpr int16_t LOADING_TEXT_X = 20;
static constexpr int16_t LOADING_TEXT_Y = 100;
static constexpr uint32_t TOUCH_STROKE_TIMEOUT_MS = 150;
static constexpr uint32_t TOUCH_LOG_INTERVAL_MS = 100;
static constexpr uint32_t TOUCH_I2C_SPEED = 400000;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
  std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(IIC_Bus, CST816T_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

struct TouchState {
  int16_t lastX = -1;
  int16_t lastY = -1;
  uint32_t lastTouchMs = 0;
  uint32_t lastLogMs = 0;
};

TouchState touchState;

void Arduino_IIC_Touch_Interrupt(void) {
  CST816T->IIC_Interrupt_Flag = true;
}

void initializeTouch() {
  while (CST816T->begin(TOUCH_I2C_SPEED) == false) {
    USBSerial.println("CST816T initialization fail");
    delay(2000);
  }
  USBSerial.println("CST816T initialization successfully");

  CST816T->IIC_Write_Device_State(CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                  CST816T->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);
  CST816T->IIC_Interrupt_Flag = false;
}

void showLoadingScreen() {
  gfx->fillScreen(BACKGROUND_COLOR);
  gfx->setCursor(LOADING_TEXT_X, LOADING_TEXT_Y);
  gfx->setTextColor(DRAW_COLOR);
  gfx->setTextSize(2);

  for (int brightness = 0; brightness <= 255; brightness++) {
    analogWrite(LCD_BL, brightness);
    gfx->println("Loading board");
    delay(3);
    gfx->setCursor(LOADING_TEXT_X, LOADING_TEXT_Y);
  }
  delay(500);
  gfx->fillScreen(BACKGROUND_COLOR);
}

void initializeDisplay() {
  gfx->begin();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  showLoadingScreen();
}

bool readTouchPoint(int16_t &touchX, int16_t &touchY) {
  if (CST816T->IIC_Interrupt_Flag != true) {
    return false;
  }
  CST816T->IIC_Interrupt_Flag = false;

  int32_t fingers = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
  if (fingers <= 0) {
    touchState.lastX = -1;
    touchState.lastY = -1;
    return false;
  }

  int32_t rawX = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
  int32_t rawY = CST816T->IIC_Read_Device_Value(CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
  if (rawX < 0 || rawY < 0) {
    return false;
  }

  touchX = constrain((int16_t)rawX, 0, LCD_WIDTH - 1);
  touchY = constrain((int16_t)rawY, 0, LCD_HEIGHT - 1);
  return true;
}

void drawTouchPoint(int16_t touchX, int16_t touchY) {
  uint32_t now = millis();

  // When two touch points are close in time, connect them so the stroke feels continuous.
  if (touchState.lastX >= 0 && touchState.lastY >= 0 &&
      (now - touchState.lastTouchMs) <= TOUCH_STROKE_TIMEOUT_MS) {
    gfx->drawLine(touchState.lastX, touchState.lastY, touchX, touchY, DRAW_COLOR);
  }
  gfx->fillCircle(touchX, touchY, DRAW_RADIUS, DRAW_COLOR);

  touchState.lastX = touchX;
  touchState.lastY = touchY;
  touchState.lastTouchMs = now;

  if ((now - touchState.lastLogMs) >= TOUCH_LOG_INTERVAL_MS) {
    USBSerial.printf("Touch X:%d Y:%d\n", touchX, touchY);
    touchState.lastLogMs = now;
  }
}

void setup() {
  USBSerial.begin(115200);
  initializeTouch();
  initializeDisplay();
}

void loop() {
  int16_t touchX = 0;
  int16_t touchY = 0;

  if (!readTouchPoint(touchX, touchY)) {
    return;
  }
  drawTouchPoint(touchX, touchY);
}
