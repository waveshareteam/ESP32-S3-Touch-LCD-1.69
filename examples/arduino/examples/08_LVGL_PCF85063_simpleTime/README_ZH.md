# 08_LVGL_PCF85063_simpleTime

[English](README.md) | [简体中文](README_ZH.md)

## 功能

PCF85063 RTC 与 LVGL 界面。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

屏幕显示 RTC 时间和日期，USB 串口每秒输出对应数据。

## 常见问题

setup() 定义了初始日期和时间，但未将其写入 RTC；显示内容来自 RTC 已保存的时间。
