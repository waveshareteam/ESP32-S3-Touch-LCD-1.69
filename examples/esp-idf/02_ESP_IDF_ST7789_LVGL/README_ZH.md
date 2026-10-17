# 02_ESP_IDF_ST7789_LVGL

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 显示与电容触摸。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

启用[仓库首页](../../../README_ZH.md)列出的 ESP-IDF 环境，在本示例目录中执行：

```bash
idf.py set-target esp32s3 build flash monitor
```

## 预期现象

LCD 显示 LVGL 控件演示，并响应触摸操作。

## 常见问题

首次编译需要联网下载 main/idf_component.yml 中声明的托管组件。
