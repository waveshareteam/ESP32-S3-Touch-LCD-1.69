# 04_GFX_ESPWiFiAnalyzer

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Wi-Fi scanning and channel visualization.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Install the Arduino-ESP32 version listed in the [repository README](../../../../README.md), set the sketchbook location to `examples/arduino/`, and select **ESP32S3 Dev Module** with **Flash Size: 16MB (128Mb)** and **PSRAM: OPI PSRAM**, matching [product.json](../../../../.github/product.json). Open the `.ino` file in this directory and upload it.

## Expected Result

The display shows a scanning message, followed by channel curves and nearby network information.

## Troubleshooting

Scanning is synchronous and can take several seconds; no network credentials are required.
