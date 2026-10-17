<div align="center">
  <h1>ESP32-S3-Touch-LCD-1.69</h1>
  <p><strong>ESP32-S3 1.69 英寸 240 × 280 SPI LCD 触摸开发板</strong></p>
  <p>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/actions/workflows/build.yml"><img alt="构建示例" src="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/actions/workflows/build.yml/badge.svg"></a>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest"><img alt="最新版本" src="https://img.shields.io/github/v/release/waveshareteam/ESP32-S3-Touch-LCD-1.69"></a>
    <a href="LICENSE"><img alt="许可证" src="https://img.shields.io/github/license/waveshareteam/ESP32-S3-Touch-LCD-1.69"></a>
  </p>
  <p><a href="README.md">English</a> | <strong>简体中文</strong></p>
  <p>
    <a href="https://www.waveshare.com/esp32-s3-touch-lcd-1.69.htm">🌐 产品页</a> &middot;
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest">📦 固件</a> &middot;
    <a href="examples/esp-idf/">🧩 ESP-IDF</a> &middot;
    <a href="examples/arduino/examples/">🔧 Arduino</a> &middot;
    <a href="hardware/">📚 资料</a>
  </p>
  <img src="docs/images/esp32-s3-touch-lcd-1.69.jpg" alt="ESP32-S3-Touch-LCD-1.69 开发板" width="500">
</div>

---

## ✨ 概述

本仓库提供 Waveshare ESP32-S3-Touch-LCD-1.69 的 ESP-IDF 和 Arduino 示例、固件下载入口及硬件资料。开发板集成 LCD、电容触摸、六轴 IMU、实时时钟和锂电池管理，适用于可穿戴设备及交互式控制面板。

| 功能 | 器件 / 接口 |
| --- | --- |
| MCU | ESP32-S3R8，双核 Xtensa LX7，主频 240 MHz |
| 存储 | 16 MB Flash 和 8 MB PSRAM |
| 无线连接 | 2.4 GHz Wi-Fi 和 Bluetooth 5 LE |
| 显示屏 | 1.69 英寸 240 × 280 SPI LCD，采用 ST7789V2 |
| 触摸 | CST816T 电容触摸控制器，通过 I2C 通信 |
| 运动传感器 | QMI8658C 六轴 IMU，通过 I2C 通信 |
| 实时时钟 | PCF85063ATL，通过 I2C 通信 |
| 电源 | ETA6098 单节锂电池充放电管理和电池电压采样 |
| 用户接口 | 板载蜂鸣器、USB Type-C、BOOT 按键和电源按键 |
| 扩展接口 | 5 V、3.3 V、GND、I2C、UART 和 GPIO 信号 |
| 板级支持 | 提供可通过组件管理器使用的 BSP：[`waveshare/esp32_s3_touch_lcd_1_69`](https://github.com/waveshareteam/Waveshare-ESP32-components/tree/master/bsp/esp32_s3_touch_lcd_1_69) |

## 📦 快速开始

从[最新版本](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest)下载所需示例和框架版本对应的固件包。

1. 下载所需示例的 ZIP 固件包。
2. 解压后执行 `python -m pip install esptool`，安装烧录工具。
3. 通过 USB 连接开发板。
4. 按固件包内 README 的说明运行烧录脚本。
5. 若开发板未自动重启，手动复位。

出厂固件（恢复出厂演示）：从 [Releases 页面](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases)下载文件名包含 `FactoryOnly` 的文件。

## 🧪 示例

### ESP-IDF

| 示例 | 功能 |
| --- | --- |
| [01_ESP_IDF_ST7789](examples/esp-idf/01_ESP_IDF_ST7789/) | ST7789V2 LCD 初始化 |
| [02_ESP_IDF_ST7789_LVGL](examples/esp-idf/02_ESP_IDF_ST7789_LVGL/) | LVGL 显示与 CST816T 触摸集成 |
| [03_PCF85063](examples/esp-idf/03_PCF85063/) | PCF85063 实时时钟 |
| [04_QMI8658](examples/esp-idf/04_QMI8658/) | QMI8658 六轴 IMU |

### Arduino

| 示例 | 功能 |
| --- | --- |
| [01_HelloWorld](examples/arduino/examples/01_HelloWorld/) | Arduino GFX 显示初始化 |
| [02_Drawing_board](examples/arduino/examples/02_Drawing_board/) | 电容触摸画板 |
| [03_GFX_AsciiTable](examples/arduino/examples/03_GFX_AsciiTable/) | GFX 文本与字符渲染 |
| [04_GFX_ESPWiFiAnalyzer](examples/arduino/examples/04_GFX_ESPWiFiAnalyzer/) | Wi-Fi 扫描与信道可视化 |
| [05_GFX_Clock](examples/arduino/examples/05_GFX_Clock/) | 图形时钟渲染 |
| [06_GFX_PCF85063_simpleTime](examples/arduino/examples/06_GFX_PCF85063_simpleTime/) | PCF85063 RTC 与 GFX 显示 |
| [07_LVGL_Measuring_voltage](examples/arduino/examples/07_LVGL_Measuring_voltage/) | LVGL 电池电压监测 |
| [08_LVGL_PCF85063_simpleTime](examples/arduino/examples/08_LVGL_PCF85063_simpleTime/) | PCF85063 RTC 与 LVGL 界面 |
| [09_LVGL_Keys_Bee](examples/arduino/examples/09_LVGL_Keys_Bee/) | 按键手势与蜂鸣器反馈 |
| [10_LVGL_QMI8658_ui](examples/arduino/examples/10_LVGL_QMI8658_ui/) | LVGL IMU 数据可视化 |
| [11_LVGL_Arduino](examples/arduino/examples/11_LVGL_Arduino/) | LVGL 控件及显示和触摸输入 |

草图使用的库位于 [`examples/arduino/libraries/`](examples/arduino/libraries/)。

## 🛠️ 开发环境

| 开发框架 | 版本 | 示例数 |
| --- | --- | ---: |
| ESP-IDF | `v5.5.5` | 4 |
| ESP-IDF | `v6.0.2` | 4 |
| Arduino-ESP32 | `3.3.11` | 11 |

## 📚 资料

- [原理图](hardware/schematic/)
- [机械尺寸图](hardware/mechanical/)
- [引脚图](docs/images/ESP32-S3-Touch-LCD-1.69-Pinout.webp)
- [产品页](https://www.waveshare.com/esp32-s3-touch-lcd-1.69.htm)
- [Wiki](https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/)

## 🤝 支持

- 示例或仓库问题：[提交 issue](https://gitee.com/waveshare/ESP32-S3-Touch-LCD-1.69/issues)。
- 产品使用、硬件或订单问题：[微雪技术支持](https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/Technical-Support/)。

## 📄 许可证

Apache License 2.0，详见 [LICENSE](LICENSE)。
