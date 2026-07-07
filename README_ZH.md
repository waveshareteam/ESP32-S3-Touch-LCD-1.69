# Waveshare ESP32-S3-Touch-LCD-1.69

[English](README.md)

ESP32-S3-Touch-LCD-1.69 是一款基于 ESP32-S3R8（双核 Xtensa LX7 @ 240 MHz，8 MB PSRAM，16 MB Flash）的紧凑型人机交互开发板，支持 2.4 GHz Wi-Fi 和 Bluetooth 5 LE。板载 1.69 英寸 240 × 280 电容触摸 LCD（ST7789V2 + CST816T）、QMI8658C 六轴 IMU、PCF85063 RTC、蜂鸣器、锂电池充放电管理、USB Type-C 以及 UART/GPIO 扩展接口，适用于打造紧凑型智能终端、可穿戴设备和交互控制面板。Arduino 与 ESP-IDF 示例覆盖 LCD、触摸、传感器、RTC 和 LVGL 演示。

- [购买链接](https://www.waveshare.net/shop/ESP32-S3-Touch-LCD-1.69.htm)
- [产品文档](https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/)

<img src="assets/540px-ESP32-S3-Touch-LCD-1.69-details-1.jpg" alt="Waveshare ESP32-S3-Touch-LCD-1.69" width="500">

## 仓库结构

本仓库提供 ESP32-S3-Touch-LCD-1.69 的示例程序、Arduino 库、出厂固件和硬件设计文件。

```
.
├── .github/workflows/     # ESP-IDF 与 Arduino 示例 CI
├── assets/                # README 使用的产品图片
├── config/                # 共享 ESP-IDF 配置覆盖
├── docs/                  # CI、固件与组件说明
├── examples/              # 示例程序
│   ├── Arduino/           # Arduino 示例与内置库
│   └── ESP-IDF/           # ESP-IDF 工程
├── firmware/              # 预编译出厂固件（.bin）
├── hardware/              # 原理图、引脚图和尺寸图
├── releases/              # CI 固件包说明与忽略的生成输出
└── HARDWARE_REFERENCE.md  # 硬件速查文件
```

## 快速开始

预编译固件位于 [`firmware/`](firmware)。构建环境、烧录步骤、引脚映射及配置说明请参阅[产品文档页面](https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/)。

如需面向开发者和 AI 编程工具的结构化硬件速查（涵盖板载外设、GPIO 分配、I2C 地址和扩展接口信号），请参考 [HARDWARE_REFERENCE_ZH.md](HARDWARE_REFERENCE_ZH.md)。

## 持续集成

GitHub Actions 会验证第一方 ESP-IDF 和 Arduino 示例。内置库自身的示例不会作为产品 CI 的构建目标。CI 矩阵、固件产物策略和组件迁移说明请参阅 [docs/ci.md](docs/ci.md)、[docs/firmware.md](docs/firmware.md) 和 [docs/components.md](docs/components.md)。

## 贡献

我们欢迎您的贡献！您可以通过以下方式提供帮助：

1. Fork 本仓库。
2. 为您的新功能或 Bug 修复创建一个新分支。
3. 提交您的更改并附上清晰的描述。
4. 提交 Pull Request 以供审核。

## 问题与支持

请创建 [Issue](https://gitee.com/waveshare/esp32-s3-touch-lcd-1.69/issues) 并提供详细信息，或联系微雪团队并提供订单号以获取技术支持。

## 许可

本仓库遵循 Apache License 2.0 许可。详情请参阅 [LICENSE](LICENSE) 文件。

---

感谢您使用微雪电子产品！🚀
