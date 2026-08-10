<div align="center">
  <h1>ESP32-S3-Touch-LCD-1.69</h1>
  <p><strong>ESP32-S3 1.69 英寸 240 × 280 SPI LCD 触摸开发板</strong></p>
  <p>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/actions/workflows/examples.yml"><img alt="构建示例" src="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/actions/workflows/examples.yml/badge.svg"></a>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest"><img alt="最新版本" src="https://img.shields.io/github/v/release/waveshareteam/ESP32-S3-Touch-LCD-1.69"></a>
    <a href="LICENSE"><img alt="许可证" src="https://img.shields.io/github/license/waveshareteam/ESP32-S3-Touch-LCD-1.69"></a>
  </p>
  <p>
    <a href="README.md">English</a> ·
    <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-LCD-1.69.htm">🌐 产品页面</a> ·
    <a href="https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/">📚 产品文档</a> ·
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest">📦 固件发布</a> ·
    <a href="examples/esp-idf/">🧩 ESP-IDF 示例</a> ·
    <a href="examples/arduino/">🔧 Arduino 示例</a>
  </p>
  <img src="assets/540px-ESP32-S3-Touch-LCD-1.69-details-1.jpg" alt="Waveshare ESP32-S3-Touch-LCD-1.69" width="500">
</div>

---

## ✨ 概述

本仓库提供适用于 Waveshare ESP32-S3-Touch-LCD-1.69 的第一方示例、
基于源码构建且可直接烧录的固件包、出厂恢复固件、原理图、引脚图和机械尺寸资料。

该开发板集成 ESP32-S3、紧凑型彩色 LCD、电容触摸、运动传感器、实时时钟、
电池管理和扩展接口，适用于可穿戴设备、智能终端和交互式控制面板。

## 🖥️ 硬件概览

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
| 硬件文件 | [原理图](hardware/schematics/)、[引脚图](hardware/pinout/)和[机械尺寸图](hardware/dimensions/) |

完整的 GPIO 分配和 I2C 地址请参阅
[硬件参考](HARDWARE_REFERENCE_ZH.md)。

## 📦 固件发布

体验示例最快的方式是从
[最新版本](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/releases/latest)
下载可直接烧录的固件包。

1. 下载所需示例和框架版本对应的 `.zip` 压缩包。
2. 解压文件，并使用 `python -m pip install esptool` 安装 esptool。
3. 通过 USB 连接开发板。
4. 在 Windows 上运行 `flash.bat COMx`，或在 Linux 上运行 `./flash.sh /dev/ttyACM0`。
5. 如果开发板没有自动重启，请手动复位。

> [!NOTE]
> 合并后的固件镜像从偏移地址 `0x0` 开始烧录。每个固件包还包含原始二进制文件、
> 烧录参数、辅助脚本和固件清单。

[`firmware/`](firmware/) 中的出厂恢复镜像与基于源码构建的示例固件包相互独立。
详情请参阅[固件和出厂恢复](docs/firmware_ZH.md)。

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
| [01_HelloWorld](examples/arduino/01_HelloWorld/) | Arduino GFX 显示初始化 |
| [02_Drawing_board](examples/arduino/02_Drawing_board/) | 电容触摸画板 |
| [03_GFX_AsciiTable](examples/arduino/03_GFX_AsciiTable/) | GFX 文本与字符渲染 |
| [04_GFX_ESPWiFiAnalyzer](examples/arduino/04_GFX_ESPWiFiAnalyzer/) | Wi-Fi 扫描与信道可视化 |
| [05_GFX_Clock](examples/arduino/05_GFX_Clock/) | 图形时钟渲染 |
| [06_GFX_PCF85063_simpleTime](examples/arduino/06_GFX_PCF85063_simpleTime/) | PCF85063 RTC 与 GFX 显示 |
| [07_LVGL_Measuring_voltage](examples/arduino/07_LVGL_Measuring_voltage/) | LVGL 电池电压监测 |
| [08_LVGL_PCF85063_simpleTime](examples/arduino/08_LVGL_PCF85063_simpleTime/) | PCF85063 RTC 与 LVGL 界面 |
| [09_LVGL_Keys_Bee](examples/arduino/09_LVGL_Keys_Bee/) | 按键手势与蜂鸣器反馈 |
| [10_LVGL_QMI8658_ui](examples/arduino/10_LVGL_QMI8658_ui/) | LVGL IMU 数据可视化 |
| [11_LVGL_Arduino](examples/arduino/11_LVGL_Arduino/) | LVGL 控件及显示和触摸输入 |

内置 Arduino 库位于
[`examples/arduino/libraries/`](examples/arduino/libraries/)。这些库的上游示例不会纳入产品 CI 构建矩阵。

## 🛠️ 支持的工具链

| 开发框架 | 版本 | 固件构建数 |
| --- | --- | ---: |
| ESP-IDF | `v5.5.5` | 4 |
| ESP-IDF | `v6.0.2` | 4 |
| Arduino-ESP32 | `3.3.11` | 11 |

[示例构建工作流](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/actions/workflows/examples.yml)
会为完整矩阵运行 2 个示例发现任务和 19 个固件构建任务。每次成功构建都会打包为可烧录的固件产物。
矩阵和手动触发方式的详细说明请参阅[持续集成](docs/ci_ZH.md)。

## 🗂️ 仓库结构

| 路径 | 用途 |
| --- | --- |
| [`examples/esp-idf/`](examples/esp-idf/) | 第一方 ESP-IDF 工程 |
| [`examples/arduino/`](examples/arduino/) | 第一方 Arduino 示例和内置库 |
| [`firmware/`](firmware/) | 出厂烧录和恢复固件 |
| [`releases/`](releases/) | 固件打包、产物下载和发布工具 |
| [`hardware/`](hardware/) | 原理图、引脚图和机械尺寸参考文件 |
| [`config/`](config/) | 预留的共享 ESP-IDF 配置覆盖目录 |
| [`docs/`](docs/) | 仓库、CI、组件和固件说明 |
| [`assets/`](assets/) | 文档使用的产品图片 |

## 📚 文档

- [产品文档](https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/)
- [硬件参考](HARDWARE_REFERENCE_ZH.md)
- [仓库结构](docs/repository-structure_ZH.md)
- [持续集成](docs/ci_ZH.md)
- [组件](docs/components_ZH.md)
- [固件和出厂恢复](docs/firmware_ZH.md)
- [发布工具](releases/README_ZH.md)

## 🤝 支持与贡献

欢迎提交贡献和可复现的问题报告。请提供示例路径、框架版本、复现步骤、预期行为、
实际行为以及相关串口日志。

- [贡献指南](CONTRIBUTING_ZH.md)
- [技术支持](SUPPORT_ZH.md)
- [安全策略](SECURITY_ZH.md)
- [提交 Issue](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/issues/new/choose)

## 📄 许可证

本仓库基于 Apache License 2.0 许可。详情请参阅
[LICENSE](LICENSE)。
