# 发布脚本

[English](README.md)

此目录包含将示例构建输出打包为可烧录归档的辅助脚本。仅在已构建第一方示例后运行打包脚本，
并使用对应的框架版本，例如 ESP-IDF `v5.5.5`、`v6.0.2` 或 Arduino-ESP32 `3.3.11`。
Arduino 构建使用 `esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi`，与本开发板的
16 MB Flash 和 8 MB OPI PSRAM 相匹配。

示例 CI 产物与 `firmware/` 中的出厂恢复镜像相互独立；不要重新打包、替换或将出厂镜像作为
CI 输出上传。
