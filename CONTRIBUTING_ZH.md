# 贡献指南

[English](CONTRIBUTING.md)

感谢改进本仓库。较大的行为变更请先建立 issue；变更应限于受影响的示例、文档、工作流或支持文件，
并在 PR 中使用仓库相对路径和 GitHub Actions 验证记录。

请说明硬件修订、受影响示例和框架版本以及验证矩阵；标明托管组件、引脚/BSP 或出厂固件影响，
并列出尚存 TODO。源码构建的 CI 产物与出厂恢复固件是不同的交付面。

第一方 ESP-IDF 示例在 `examples/esp-idf/`，第一方 Arduino sketch 在 `examples/arduino/`。
内置 Arduino 库的内部示例不是产品 CI 目标。不要替换或重新生成 `firmware/` 中的出厂二进制。
