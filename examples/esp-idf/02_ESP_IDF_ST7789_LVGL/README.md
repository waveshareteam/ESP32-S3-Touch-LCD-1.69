# ESP32-S3 ST7789 LVGL example

[简体中文](README_ZH.md)

This ESP-IDF project targets the ESP32-S3-Touch-LCD-1.69. It integrates the
ST7789 display, LVGL display port, and CST816T capacitive touch component used
by this board. It is a product example, not a generic ESP-IDF project template.

The project configuration is in `sdkconfig.defaults`; application code and its
component manifest are under `main/`. Build it from the repository root with
the selected ESP-IDF environment and target `esp32s3`. Refer to the
[hardware reference](../../../HARDWARE_REFERENCE.md) for board-level signals.
