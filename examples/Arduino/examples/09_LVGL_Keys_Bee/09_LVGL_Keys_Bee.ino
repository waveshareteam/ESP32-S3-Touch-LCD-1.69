#error "按键失灵时,请修改USE_NEW_PIN_CONFIG为1以使用新的引脚配置。运行代码时,请注释此行,If the key fails, change USE_NEW_PIN_CONFIG to 1 to use the new pin configuration. Comment this line as you run the code"

#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "lv_conf.h"
#include "demos/lv_demos.h"
#include "pin_config.h"
#include <Wire.h>
#include "HWCDC.h"

HWCDC USBSerial;
/*To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 *You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 Note that the `lv_examples` library is for LVGL v7 and you shouldn't install it for this version (since LVGL v8)
 as the examples and demos are now part of the main LVGL library. */

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
#define USE_NEW_PIN_CONFIG 0

#if (USE_NEW_PIN_CONFIG)
// New pin configuration
const int inputPin = 40;   // Define GPIO40 as input
const int outputPin = 41;  // Define GPIO41 as output
const int beePin = 42;     // Define GPIO42 for buzzer
#else
// Existing pin configuration
const int inputPin = 36;   // Define GPIO36 as input
const int outputPin = 35;  // Define GPIO35 as output
const int beePin = 33;     // Define GPIO33 for buzzer
#endif

bool buttonState = false;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long lastClickTime = 0;
unsigned long clickInterval = 500;       // Double-click interval
unsigned long longPressDuration = 1000;  // Long press duration

bool longPressDetected = false;
bool doubleClickDetected = false;
bool clickDetected = false;

/*Change to your screen resolution*/
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 280;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

lv_obj_t *label;  // Global label object

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);

Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
                                      0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);


#if LV_USE_LOG != 0
/* USBSerial debugging */
void my_print(const char *buf) {
  USBSerial.printf(buf);
  USBSerial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void example_increase_lvgl_tick(void *arg) {
  /* Tell LVGL how many milliseconds has elapsed */
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static uint8_t count = 0;
void example_increase_reboot(void *arg) {
  count++;
  if (count == 30) {
    esp_restart();
  }
}

void setup() {
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(beePin, OUTPUT);
  digitalWrite(outputPin, HIGH);  // Initialize output pin to HIGH

  gfx->begin();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  USBSerial.println(LVGL_Arduino);
  USBSerial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif



  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };

  const esp_timer_create_args_t reboot_timer_args = {
    .callback = &example_increase_reboot,
    .name = "reboot"
  };

  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

  // esp_timer_handle_t reboot_timer = NULL;
  // esp_timer_create(&reboot_timer_args, &reboot_timer);
  // esp_timer_start_periodic(reboot_timer, 2000 * 1000);

  label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Initializing...");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  USBSerial.println("Setup done");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  int reading = digitalRead(inputPin);
  // Debounce processing
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        // Operation when button is pressed
        unsigned long now = millis();
        if (now - lastClickTime < clickInterval && !longPressDetected) {
          lv_label_set_text(label, "Double Click");
          // Double click detected
          printf("Double Click\n");
          doubleClickDetected = true;
        } else {
          // Single click detected
          if (!longPressDetected && !doubleClickDetected) {
            lv_label_set_text(label, "Single Click");
            printf("Single Click\n");
            clickDetected = true;
          }
        }
        lastClickTime = now;

        delay(100);  // Used to eliminate button noise
      } else {
        // Operation when button is released
        if (longPressDetected) {
          lv_label_set_text(label, "Long Press Released");

          // Set GPIO35 output to low level after long pressing the button and releasing it
          printf("Long Press Released\n");
          noTone(beePin);
          digitalWrite(outputPin, LOW);  // Set GPIO35 output to low level
          longPressDetected = false;     // Reset long press detection flag
        }
        clickDetected = false;
        doubleClickDetected = false;
      }
    }
  }

  // Check if the button is in long press state
  if (buttonState == LOW && (millis() - lastDebounceTime >= longPressDuration)) {
    lv_label_set_text(label, "Long Press");
    // Long press button
    printf("Long Press\n");
    tone(beePin, 2000);
    longPressDetected = true;     // Set long press detection flag
    clickDetected = false;        // Reset single click detection flag
    doubleClickDetected = false;  // Reset double click detection flag
  }
  lastButtonState = reading;
}
