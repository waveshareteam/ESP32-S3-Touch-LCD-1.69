# ESP32-S3-Touch-LCD-1.69 Hardware Reference

> Hardware quick reference for developers and AI tools. The content is cross-checked against the official Wiki, schematic V2.1, and the pin definitions in the current project.

## 1. Board Overview

- Product: `ESP32-S3-Touch-LCD-1.69`, SKU `27350`
- Main controller: `ESP32-S3R8`, dual-core LX7, Wi-Fi / BLE, 8 MB PSRAM
- Flash: 16 MB
- Display: 1.69-inch LCD, `240 x 280`, typically RGB565
- Main onboard resources: ST7789V2 LCD, CST816T touch, QMI8658C IMU, PCF85063 RTC, buzzer, lithium battery charge/discharge, battery voltage sampling, USB, UART, expansion header

## 2. Onboard Peripherals

| Module | Device / Function | Interface | Address / Parameters | GPIO / Signal |
|---|---|---|---|---|
| LCD | ST7789V2 | 4-wire SPI | 240 x 280, typically RGB565, MISO not used | DC=GPIO4, CS=GPIO5, SCL=GPIO6, MOSI=GPIO7, RST=GPIO8, BL=GPIO15 |
| Touch | CST816T capacitive touch | I2C | 7-bit address `0x15` | SCL=GPIO10, SDA=GPIO11, RST=GPIO13, INT=GPIO14 |
| IMU | QMI8658C 6-axis sensor | I2C | 7-bit address `0x6B` | SCL=GPIO10, SDA=GPIO11, INT2=GPIO38 |
| RTC | PCF85063ATL | I2C | Common 7-bit address `0x51`, 32.768 kHz crystal | SCL=GPIO10, SDA=GPIO11, INT=GPIO39 |
| Buzzer | Onboard buzzer | GPIO / PWM | - | GPIO42 |
| Battery sampling | B+ divided to ADC | ADC | R3=200K, R7=100K | GPIO1 / BAT_ADC |
| Power control | SYS_OUT / SYS_EN | GPIO | PWR / Key2 power-function circuit | SYS_OUT=GPIO40, SYS_EN=GPIO41 |
| USB Type-C | ESP32-S3 native USB | USB | Download, logs | USB_N=GPIO19, USB_P=GPIO20 |
| UART0 | Default serial port | UART | Debug / expansion pads | U0TXD=GPIO43, U0RXD=GPIO44 |
| Charge management | ETA6098 | Power | Single-cell lithium battery charge/discharge | PH1.25 / M1.25 battery connector |
| 3.3 V LDO | ME6217C33M5G | Power | System 3.3 V | VCC3V3 |

## 3. Complete GPIO Assignment

Pins occupied by onboard circuits or explicitly broken out are listed in ascending GPIO order.

| GPIO | Signal Name | Connected To | Notes |
|---:|---|---|---|
| GPIO0 | BOOT / Key1 | BOOT button | Strapping pin; button enters download mode |
| GPIO1 | BAT_ADC | Battery voltage divider sampling | Divider network R3=200K / R7=100K |
| GPIO2 | PAD_GPIO2 | Reserved pad / header | Expansion GPIO2 |
| GPIO3 | PAD_GPIO3 | Reserved pad / header | Expansion GPIO3 |
| GPIO4 | LCD_DC | ST7789V2 data/command | - |
| GPIO5 | LCD_CS | ST7789V2 chip select | - |
| GPIO6 | LCD_SCL | ST7789V2 SPI clock | - |
| GPIO7 | LCD_MOSI / LCD_SDA | ST7789V2 SPI data | LCD write-only SPI data line, MISO not used |
| GPIO8 | LCD_RST | ST7789V2 reset | - |
| GPIO10 | ESP32_SCL | Shared I2C SCL for touch + IMU + RTC; also broken out to expansion header | See Section 4 for I2C devices |
| GPIO11 | ESP32_SDA | Shared I2C SDA for touch + IMU + RTC; also broken out to expansion header | See Section 4 for I2C devices |
| GPIO13 | TP_RST | CST816T touch reset | - |
| GPIO14 | TP_INT | CST816T touch interrupt | - |
| GPIO15 | LCD_BL | LCD backlight control | - |
| GPIO17 | PAD_GPIO17 | Reserved pad / header | Expansion GPIO17 |
| GPIO18 | PAD_GPIO18 | Reserved pad / header | Expansion GPIO18 |
| GPIO19 | USB_N | USB Type-C D- | ESP32-S3 native USB |
| GPIO20 | USB_P | USB Type-C D+ | ESP32-S3 native USB |
| GPIO38 | QMI_INT2 | QMI8658C INT2 | - |
| GPIO39 | RTC_INT | PCF85063 RTC interrupt | - |
| GPIO40 | SYS_OUT | System power-control network | Related to the PWR / Key2 power-function circuit |
| GPIO41 | SYS_EN | System power-control network | Related to the PWR / Key2 power-function circuit |
| GPIO42 | Buzz | Onboard buzzer drive | Can be used for PWM / tone output |
| GPIO43 | U0TXD | Reserved UART TX pad / header | Expansion TX |
| GPIO44 | U0RXD | Reserved UART RX pad / header | Expansion RX |

## 4. Onboard I2C Devices

| Device | Component | I2C Address | Extra Pins |
|---|---|---:|---|
| Touch | CST816T | `0x15` | RST=GPIO13, INT=GPIO14 |
| IMU | QMI8658C | `0x6B` | INT2=GPIO38 |
| RTC | PCF85063ATL | `0x51` | INT=GPIO39 |

Shared I2C bus: `SCL=GPIO10`, `SDA=GPIO11`.

## 5. Expansion Header

| Type | Signals |
|---|---|
| Power | `5V` / `3V3` / `GND` |
| I2C | `SCL(GPIO10)` / `SDA(GPIO11)` |
| UART | `TX(GPIO43)` / `RX(GPIO44)` |
| GPIO | `GPIO2` / `GPIO3` / `GPIO17` / `GPIO18` |

## 6. Usage Notes

- When connecting external I2C devices, avoid address conflicts with `0x15`, `0x6B`, and `0x51`.
- `GPIO19/GPIO20` are already connected to USB Type-C and are not recommended for use as general-purpose GPIO.
- `GPIO0` is the BOOT pin, and `EN/CHIP_PU` is the reset signal; they are not recommended as normal user inputs.
- `GPIO33` to `GPIO37` are not on the expansion header, and should not be marked in maps as occupied by onboard PSRAM.
- SHTC3 / temperature and humidity sensor is not a default onboard resource for this board. If a derivative project includes `SHT_SCL/SHT_SDA`, confirm against the corresponding schematic, BOM, and physical hardware.

## 7. References

| Type | Link |
|---|---|
| Chinese Wiki | https://docs.waveshare.net/ESP32-S3-Touch-LCD-1.69/ |
| English Documentation | https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69 |
| Schematic V2.1 | https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.69/ESP32-S3-Touch-LCD-1.69_V2.1.pdf |
| Board Support Package (BSP) | https://github.com/waveshareteam/Waveshare-ESP32-components/tree/master/bsp/esp32_s3_touch_lcd_1_69 |
