# ESP32-S3-LCD-1.69 固件

[English](README.md) | [简体中文](README_ZH.md)

此出厂固件用于恢复 ESP32-S3-LCD-1.69 非触摸版（SKU 27932）的开箱演示。ESP32-S3-Touch-LCD-1.69 请使用[触摸版出厂固件](../README_ZH.md)。

| 文件 | 适用硬件 | 烧录偏移 |
| --- | --- | --- |
| [ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin](ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin) | ESP32-S3-LCD-1.69（非触摸版） | `0x0` |

此合并 BIN 与[官方示例包](https://docs.waveshare.com/ESP32-S3-LCD-1.69/Resources-And-Documents)内的出厂镜像一致，版本日期取自镜像内记录。镜像声明 4 MB Flash，开发板实物配有 16 MB Flash。

安装 esptool，将 `<PORT>` 替换为开发板的串口号，然后运行以下命令。擦除步骤会清除板上已有数据。

```bash
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase-flash
python -m esptool --chip esp32s3 --port <PORT> write-flash 0x0 ESP32-S3-LCD-1.69_FactoryOnly_20230612.bin
```
