# 10_LVGL_QMI8658_ui

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 加速度与角速度曲线。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

移动或转动开发板时，两张曲线图随之变化，USB 串口输出加速度和角速度数据。

## 常见问题

传感器初始化失败时，程序停留在等待循环中；示例使用仓库内置的 SensorLib。
