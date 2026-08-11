# 仓库结构

[English](repository-structure.md)

- `examples/esp-idf/`：第一方 ESP-IDF 工程。
- `examples/arduino/`：第一方 Arduino sketch；`libraries/` 为内置上游代码。
- `config/`：实际被多个示例使用时的共享配置覆盖。
- `docs/`：维护者文档。
- `firmware/`：不可变出厂二进制，不进入示例 CI。
- `releases/`：产物打包和下载辅助工具。
- `hardware/schematics/`：公开原理图文件。
- `assets/`：文档使用的产品图片。

CI 仅构建第一方示例，绝不构建内置库示例或出厂固件。
