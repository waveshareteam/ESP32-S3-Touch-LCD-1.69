# ESP32-S3-LCD-1.69 firmware

[English](README.md) | [简体中文](README_ZH.md)

This factory firmware restores the out-of-box demo on the ESP32-S3-LCD-1.69 non-touch board (SKU 27932). For ESP32-S3-Touch-LCD-1.69, use the [touch model firmware](../README.md).

| File | Hardware | Flash offset |
| --- | --- | --- |
| [ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin](ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin) | ESP32-S3-LCD-1.69 (non-touch) | `0x0` |

The merged BIN matches the factory image in the [official example package](https://docs.waveshare.com/ESP32-S3-LCD-1.69/Resources-And-Documents). The version uses the date recorded in the image. The image declares 4 MB of Flash and the board has 16 MB of Flash.

Install esptool, replace `<PORT>` with the board's serial port, and run the following commands. The erase step clears existing data on the board.

```bash
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase-flash
python -m esptool --chip esp32s3 --port <PORT> write-flash 0x0 ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin
```
