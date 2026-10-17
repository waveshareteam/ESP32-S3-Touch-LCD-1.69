# 01_HelloWorld

[English](README.md) | [简体中文](README_ZH.md)

## 功能

GFX 文字显示。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

以随机位置、字号和颜色显示 Hello World。

## 常见问题

启动文字显示 5 秒后开始循环绘制。
