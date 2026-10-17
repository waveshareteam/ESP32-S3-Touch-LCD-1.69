# 11_LVGL_Arduino

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 显示与触摸控件。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

屏幕显示可触摸操作的控件演示，USB 串口输出触摸坐标。

## 常见问题

启动时会等待触摸初始化成功；请配套使用仓库提供的 LVGL 配置和库。
