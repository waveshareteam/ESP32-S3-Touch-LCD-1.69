# Component Strategy

The board has a managed Waveshare BSP available as `waveshare/esp32_s3_touch_lcd_1_69` in the ESP Component Registry. That BSP is the preferred long-term direction for reusable board, display, touch, and sensor support.

Current source still contains local reusable code:

- `examples/esp-idf/01_ESP_IDF_ST7789/components/st7789`
- `examples/esp-idf/04_QMI8658/components/SensorLib`
- bundled Arduino libraries under `examples/arduino/libraries/`

This update keeps those local copies in place to avoid rewriting example source before CI establishes a baseline. After the ESP-IDF and Arduino workflows are green, migrate reusable local pieces toward managed components in a separate change.

Existing managed dependencies:

- `esp_jpeg` in `examples/esp-idf/01_ESP_IDF_ST7789/main/idf_component.yml`
- `espressif/esp_lcd_touch_cst816s`, `lvgl/lvgl`, and `espressif/esp_lvgl_port` in `examples/esp-idf/02_ESP_IDF_ST7789_LVGL/main/idf_component.yml`

TODO:

- Evaluate replacing the ST7789 and SensorLib local ESP-IDF components with managed dependencies after baseline CI results are available.
- Keep Arduino bundled libraries in product CI as library paths only; do not compile their internal examples unless library-level CI is explicitly requested.
