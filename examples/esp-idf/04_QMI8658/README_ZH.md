# 04_QMI8658

[English](README.md) | [简体中文](README_ZH.md)

## 功能

QMI8658 运动与温度数据。

## 硬件需求

ESP32-S3-Touch-LCD-1.69 开发板和 USB 数据线。

## 怎么运行

启用[仓库首页](../../../README_ZH.md)列出的 ESP-IDF 环境，在本示例目录中执行：

```bash
idf.py set-target esp32s3 build flash monitor
```

## 预期现象

串口监视器输出器件 ID、加速度、角速度、时间戳和温度。

## 常见问题

示例使用本地 SensorLib 组件，未检测到传感器时停止初始化。
