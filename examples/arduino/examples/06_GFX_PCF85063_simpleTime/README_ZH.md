# 06_GFX_PCF85063_simpleTime

[English](README.md) | [简体中文](README_ZH.md)

## 功能

PCF85063 RTC 与 GFX 显示。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

RTC 日期和时间居中分为两行显示，每秒更新一次。

## 常见问题

每次复位都会将 setup() 中的日期和时间常量写入 RTC；RTC 初始化失败后程序停止运行。
