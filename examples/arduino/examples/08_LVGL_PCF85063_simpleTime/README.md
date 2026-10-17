# 08_LVGL_PCF85063_simpleTime

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

PCF85063 RTC with an LVGL interface.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

The display shows the RTC time and date; USB serial prints the same values once per second.

## Troubleshooting

setup() defines an initial date and time but does not write them to the RTC; the display uses the stored RTC value.
