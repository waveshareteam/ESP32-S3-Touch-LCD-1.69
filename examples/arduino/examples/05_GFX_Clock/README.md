# 05_GFX_Clock

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

GFX analog clock.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

The display shows a clock face with moving hour, minute and second hands.

## Troubleshooting

Each reset starts the clock from __TIME__; the sketch does not read the onboard RTC.
