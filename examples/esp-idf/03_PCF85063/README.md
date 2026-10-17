# 03_PCF85063

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

PCF85063 RTC over I2C.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Activate an ESP-IDF version listed in the [repository README](../../../README.md), open a terminal in this example directory, and run:

```bash
idf.py set-target esp32s3 build flash monitor
```

## Expected Result

The serial monitor prints Current Time once per second after setting the RTC.

## Troubleshooting

Every startup resets the RTC to the constants passed to rtc_set_time() in app_main().
