# 09_LVGL_Keys_Bee

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Button gestures and buzzer feedback.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

With the matching pin configuration, single clicks, double clicks and long presses update the label; a long press produces a tone.

## Troubleshooting

USE_NEW_PIN_CONFIG defaults to 0; select the pin group that matches the schematic before testing the onboard button and buzzer.
