# 03_PCF85063

[English](README.md) | [简体中文](README_ZH.md)

## 功能

I2C PCF85063 RTC。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

启用[仓库首页](../../../README_ZH.md)列出的 ESP-IDF 环境，在本示例目录中执行：

```bash
idf.py set-target esp32s3 build flash monitor
```

## 预期现象

设置 RTC 后，串口监视器每秒输出一条 Current Time 日期和时间信息。

## 常见问题

每次启动都会将 RTC 重置为 app_main() 中 rtc_set_time() 指定的固定日期和时间。
