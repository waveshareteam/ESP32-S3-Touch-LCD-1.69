# 07_LVGL_Measuring_voltage

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 电池电压监测。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。测量电池电压时，需连接兼容的锂电池。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

Actual Voltage 标签持续显示根据 ADC 采样计算的电压。

## 常见问题

计算采用 3.3 V 参考电压和 200 kΩ / 100 kΩ 分压参数，未进行 ADC 校准。
