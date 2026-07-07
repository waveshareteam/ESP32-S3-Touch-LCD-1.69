#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "lv_conf.h"
#include "demos/lv_demos.h"
#include "pin_config.h"

/* Using LVGL with Arduino requires some extra steps:
 * Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

/* Change to your screen resolution */
static const uint16_t screenWidth = 240;
static const uint16_t screenHeight = 280;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];


const int voltageDividerPin = 1;  // GPIO1 pin
float vRef = 3.3;                 // Power supply voltage of ESP32-S3 (unit: volts)
float R1 = 200000.0;              // Resistance value of the first resistor (unit: ohms)
float R2 = 100000.0;              // Resistance value of the second resistor (unit: ohms)

lv_obj_t *label;  // Global label object

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);

Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
                                      0 /* rotation */, true /* IPS */, LCD_WIDTH, LCD_HEIGHT, 0, 20, 0, 0);

#if LV_USE_LOG!= 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP!= 0)
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
  Serial.begin(115200); /* prepare for possible serial debug */
  pinMode(voltageDividerPin, INPUT);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG!= 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  gfx->begin();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
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

  /* Create label */
  label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Initializing...");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  Serial.println("Setup done");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  // Read ADC value
  int adcValue = analogRead(voltageDividerPin);

  // Convert to voltage
  float voltage = (float)adcValue * (vRef / 4095.0);

  // Apply the voltage divider formula to calculate the actual voltage
  float actualVoltage = voltage * ((R1 + R2) / R2);

  // Print the actual voltage
  Serial.print("Actual Voltage: ");
  Serial.print(actualVoltage);
  Serial.println(" V");

  // Update label content
  String voltageStr = "Actual Voltage: " + String(actualVoltage) + " V";
  lv_label_set_text(label, voltageStr.c_str());
}