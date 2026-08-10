# Component Strategy

[简体中文](components_ZH.md)

The managed Waveshare BSP `waveshare/esp32_s3_touch_lcd_1_69` is the preferred
long-term direction where equivalence is established. This repository retains
the local ST7789 component in `01_ESP_IDF_ST7789` and SensorLib in `04_QMI8658`.
Current registry/Waveshare evidence does not establish semantic and board-hardware
equivalence for these uses, so neither is migrated or removed here.

Revisit a migration only with evidence for compatible component version, target
and framework support, matching board pin behavior, and the required API
semantics. A green CI compile alone is not evidence of hardware equivalence.
Bundled Arduino libraries remain library paths for product sketches; their own
upstream examples are not product CI targets.
