# 组件策略

[English](components.md)

在已证实等价的前提下，托管 BSP `waveshare/esp32_s3_touch_lcd_1_69` 是长期优先方向。
本仓库保留 `01_ESP_IDF_ST7789` 的本地 ST7789 组件和 `04_QMI8658` 的 SensorLib：当前
registry/Waveshare 资料不能证明它们在语义和本板硬件上等价，因此本次不迁移也不删除。

只有取得组件版本、目标和框架支持、板级引脚行为以及所需 API 语义相容的证据后，才重新评估迁移。
CI 编译通过本身不能证明硬件等价。内置 Arduino 库继续作为产品 sketch 的库路径；其上游示例
不属于产品 CI 目标。
