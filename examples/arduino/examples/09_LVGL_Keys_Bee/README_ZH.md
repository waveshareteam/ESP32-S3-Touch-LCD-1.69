# 09_LVGL_Keys_Bee

[English](README.md) | [简体中文](README_ZH.md)

## 功能

按键手势与蜂鸣器反馈。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

安装[仓库首页](../../../../README_ZH.md)列出的 Arduino-ESP32 版本，将项目文件夹位置设为 `examples/arduino/`，选择 **ESP32S3 Dev Module**，并按 [product.json](../../../../.github/product.json) 设置 **Flash Size: 16MB (128Mb)**、**PSRAM: OPI PSRAM**。打开本目录的 `.ino` 文件并上传。

## 预期现象

使用匹配的引脚配置时，单击、双击和长按会更新标签，长按时蜂鸣器发声。

## 常见问题

USE_NEW_PIN_CONFIG 默认为 0；测试板载按键和蜂鸣器前，应按原理图选择匹配的引脚配置组。
