# 07_LVGL_Measuring_voltage

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

LVGL battery-voltage monitor.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable. Connect a compatible lithium battery to measure its voltage.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

An Actual Voltage label continuously displays the voltage calculated from ADC samples.

## Troubleshooting

The calculation assumes a 3.3 V reference and a 200 kΩ / 100 kΩ divider; it does not apply ADC calibration.
