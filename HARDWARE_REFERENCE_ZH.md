# ESP32-S3-Touch-LCD-1.69 硬件参考

> 给开发者、AI 工具使用的硬件速查文件。内容以官方 Wiki、原理图 V2.1 和当前工程引脚定义交叉整理。

## 1. 板卡概览

- 产品：`ESP32-S3-Touch-LCD-1.69`，SKU `27350`
- 主控：`ESP32-S3R8`，双核 LX7，Wi-Fi / BLE，8 MB PSRAM
- Flash：16 MB
- 屏幕：1.69 英寸 LCD，`240 x 280`，常用 RGB565
- 主要板载资源：ST7789V2 LCD、CST816T 触摸、QMI8658C IMU、PCF85063 RTC、蜂鸣器、锂电池充放电、电池电压采样、USB、UART、扩展口

## 2. 板载外设

| 模块 | 器件 / 功能 | 接口 | 地址 / 参数 | GPIO / 信号 |
|---|---|---|---|---|
| LCD | ST7789V2 | 4-wire SPI | 240 x 280，常用 RGB565，MISO 未使用 | DC=GPIO4，CS=GPIO5，SCL=GPIO6，MOSI=GPIO7，RST=GPIO8，BL=GPIO15 |
| 触摸 | CST816T 电容触摸 | I2C | 7-bit 地址 `0x15` | SCL=GPIO10，SDA=GPIO11，RST=GPIO13，INT=GPIO14 |
| IMU | QMI8658C 6 轴传感器 | I2C | 7-bit 地址 `0x6B` | SCL=GPIO10，SDA=GPIO11，INT2=GPIO38 |
| RTC | PCF85063ATL | I2C | 常用 7-bit 地址 `0x51`，32.768 kHz 晶振 | SCL=GPIO10，SDA=GPIO11，INT=GPIO39 |
| 蜂鸣器 | 板载蜂鸣器 | GPIO / PWM | - | GPIO42 |
| 电池采样 | B+ 分压到 ADC | ADC | R3=200K，R7=100K | GPIO1 / BAT_ADC |
| 电源控制 | SYS_OUT / SYS_EN | GPIO | PWR / Key2 电源功能电路 | SYS_OUT=GPIO40，SYS_EN=GPIO41 |
| USB Type-C | ESP32-S3 原生 USB | USB | 下载、日志 | USB_N=GPIO19，USB_P=GPIO20 |
| UART0 | 默认串口 | UART | 调试 / 扩展焊盘 | U0TXD=GPIO43，U0RXD=GPIO44 |
| 充电管理 | ETA6098 | 电源 | 单节锂电池充放电 | PH1.25 / M1.25 电池接口 |
| 3.3 V LDO | ME6217C33M5G | 电源 | 系统 3.3 V | VCC3V3 |

## 3. GPIO 完整分配

按 GPIO 编号升序列出已被板上电路占用或明确接出的引脚。

| GPIO | 信号名 | 连接到 | 备注 |
|---:|---|---|---|
| GPIO0 | BOOT / Key1 | BOOT 按键 | Strapping pin；按键接入下载模式 |
| GPIO1 | BAT_ADC | 电池电压分压采样 | 分压网络 R3=200K / R7=100K |
| GPIO2 | PAD_GPIO2 | 预留焊盘 / 排针 | 扩展口 GPIO2 |
| GPIO3 | PAD_GPIO3 | 预留焊盘 / 排针 | 扩展口 GPIO3 |
| GPIO4 | LCD_DC | ST7789V2 数据/命令 | - |
| GPIO5 | LCD_CS | ST7789V2 片选 | - |
| GPIO6 | LCD_SCL | ST7789V2 SPI 时钟 | - |
| GPIO7 | LCD_MOSI / LCD_SDA | ST7789V2 SPI 数据 | LCD 只写 SPI 数据线，MISO 未使用 |
| GPIO8 | LCD_RST | ST7789V2 复位 | - |
| GPIO10 | ESP32_SCL | 触摸 + IMU + RTC 共享 I2C SCL；也接出到扩展口 | I2C 设备见第 4 节 |
| GPIO11 | ESP32_SDA | 触摸 + IMU + RTC 共享 I2C SDA；也接出到扩展口 | I2C 设备见第 4 节 |
| GPIO13 | TP_RST | CST816T 触摸复位 | - |
| GPIO14 | TP_INT | CST816T 触摸中断 | - |
| GPIO15 | LCD_BL | LCD 背光控制 | - |
| GPIO17 | PAD_GPIO17 | 预留焊盘 / 排针 | 扩展口 GPIO17 |
| GPIO18 | PAD_GPIO18 | 预留焊盘 / 排针 | 扩展口 GPIO18 |
| GPIO19 | USB_N | USB Type-C D- | ESP32-S3 原生 USB |
| GPIO20 | USB_P | USB Type-C D+ | ESP32-S3 原生 USB |
| GPIO38 | QMI_INT2 | QMI8658C INT2 | - |
| GPIO39 | RTC_INT | PCF85063 RTC 中断 | - |
| GPIO40 | SYS_OUT | 系统电源控制网络 | 与 PWR / Key2 电源功能电路相关 |
| GPIO41 | SYS_EN | 系统电源控制网络 | 与 PWR / Key2 电源功能电路相关 |
| GPIO42 | Buzz | 板载蜂鸣器驱动 | 可作 PWM / tone 输出 |
| GPIO43 | U0TXD | 预留 UART TX 焊盘 / 排针 | 扩展口 TX |
| GPIO44 | U0RXD | 预留 UART RX 焊盘 / 排针 | 扩展口 RX |

## 4. I2C 板载设备

| 设备 | 器件 | I2C 地址 | 额外引脚 |
|---|---|---:|---|
| 触摸 | CST816T | `0x15` | RST=GPIO13，INT=GPIO14 |
| IMU | QMI8658C | `0x6B` | INT2=GPIO38 |
| RTC | PCF85063ATL | `0x51` | INT=GPIO39 |

共享 I2C 总线：`SCL=GPIO10`，`SDA=GPIO11`。

## 5. 扩展口

| 类型 | 信号 |
|---|---|
| 电源 | `5V` / `3V3` / `GND` |
| I2C | `SCL(GPIO10)` / `SDA(GPIO11)` |
| UART | `TX(GPIO43)` / `RX(GPIO44)` |
| GPIO | `GPIO2` / `GPIO3` / `GPIO17` / `GPIO18` |

## 6. 使用注意

- 外接 I2C 设备时，需要避开 `0x15`、`0x6B`、`0x51` 地址冲突。
- `GPIO19/GPIO20` 已连接 USB Type-C，不建议当普通 GPIO 使用。
- `GPIO0` 是 BOOT 引脚，`EN/CHIP_PU` 是复位信号，不建议作为普通用户输入。
- `GPIO33` 到 `GPIO37` 不在扩展口中，也不要在 map 中写成板载 PSRAM 占用。
- SHTC3 / 温湿度传感器不作为本板默认板载资源；如派生工程出现 `SHT_SCL/SHT_SDA`，需按对应原理图、BOM 和实物确认。

## 7. 参考资料

| 类型 | 链接 |
|---|---|
| 中文 Wiki | https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/ |
| 英文文档 | https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69 |
| 原理图 V2.1 | https://gitee.com/waveshare/esp32-s3-touch-lcd-1.69/blob/main/hardware/schematics/ESP32-S3-Touch-LCD-1.69_V2.1.pdf |
| 板级支持包 (BSP) | https://github.com/waveshareteam/Waveshare-ESP32-components/tree/master/bsp/esp32_s3_touch_lcd_1_69 |
