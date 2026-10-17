# 04_QMI8658

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

QMI8658 motion and temperature data.

## Hardware Required

ESP32-S3-Touch-LCD-1.69 development board and a USB data cable.

## How to Run

Activate an ESP-IDF version listed in the [repository README](../../../README.md), open a terminal in this example directory, and run:

```bash
idf.py set-target esp32s3 build flash monitor
```

## Expected Result

The serial monitor prints the device ID, acceleration, angular velocity, timestamps and temperature.

## Troubleshooting

The example uses its local SensorLib component and stops initialization if the sensor cannot be detected.
