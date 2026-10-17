# 01_ESP_IDF_ST7789

[English](README.md) | [简体中文](README_ZH.md)

## 功能

ST7789V2 图形、字体和图片。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

启用[仓库首页](../../../README_ZH.md)列出的 ESP-IDF 环境，在本示例目录中执行：

```bash
idf.py set-target esp32s3 build flash monitor
```

## 预期现象

LCD 循环显示纯色、图形、字体和 SPIFFS 图片，串口输出文件系统及测试耗时信息。

## 常见问题

应完整烧录工程：storage 分区镜像由 font/ 生成，并通过 FLASH_IN_PROJECT 随工程一起烧录。
