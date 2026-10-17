# 01_ESP_IDF_ST7789

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

ST7789V2 graphics, fonts and images.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Activate an ESP-IDF version listed in the [repository README](../../../README.md), open a terminal in this example directory, and run:

```bash
idf.py set-target esp32s3 build flash monitor
```

## Expected Result

The LCD cycles through colors, graphics, fonts and SPIFFS images; serial logs report filesystem and test timing information.

## Troubleshooting

Use a complete project flash: the storage partition image is generated from font/ and included by FLASH_IN_PROJECT.
