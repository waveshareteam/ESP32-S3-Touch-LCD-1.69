# 02_ESP_IDF_ST7789_LVGL

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

LVGL display and capacitive touch.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Activate an ESP-IDF version listed in the [repository README](../../../README.md), open a terminal in this example directory, and run:

```bash
idf.py set-target esp32s3 build flash monitor
```

## Expected Result

The LVGL widgets demo appears on the LCD and responds to touch.

## Troubleshooting

The first build requires network access to download the managed components declared in main/idf_component.yml.
