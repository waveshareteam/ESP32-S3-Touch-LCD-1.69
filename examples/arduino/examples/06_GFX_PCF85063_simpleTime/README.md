# 06_GFX_PCF85063_simpleTime

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

PCF85063 RTC with GFX output.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

The RTC date and time appear as two centered lines and update once per second.

## Troubleshooting

Each reset writes the date and time constants in setup() to the RTC; initialization failure stops the program.
