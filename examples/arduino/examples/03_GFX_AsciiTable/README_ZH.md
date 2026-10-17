# 03_GFX_AsciiTable

[English](README.md) | [简体中文](README_ZH.md)

## 功能

GFX 字符表。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

屏幕显示静态字符表，并以十六进制标注行列索引。

## 常见问题

字符表仅在初始化时绘制一次，主循环不刷新画面。
