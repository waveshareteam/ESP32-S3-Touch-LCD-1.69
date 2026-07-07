# Waveshare ESP32-S3-Touch-LCD-1.69

[中文](README_ZH.md)

The ESP32-S3-Touch-LCD-1.69 is a compact human-machine interaction development board based on the ESP32-S3R8 (dual-core Xtensa LX7 @ 240 MHz, 8 MB PSRAM, 16 MB Flash) with 2.4 GHz Wi-Fi and Bluetooth 5 LE. It features a 1.69-inch 240 × 280 capacitive touch LCD (ST7789V2 + CST816T), a QMI8658C 6-axis IMU, a PCF85063 RTC, a buzzer, onboard lithium battery charge/discharge management, USB Type-C, and a UART/GPIO expansion header — making it ideal for building compact smart terminals, wearables, and interactive control panels. Arduino and ESP-IDF examples cover LCD, touch, sensors, RTC, and LVGL.

- [Purchase Link](https://www.waveshare.com/esp32-s3-touch-lcd-1.69.htm)
- [Documentation](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69)

<img src="assets/540px-ESP32-S3-Touch-LCD-1.69-details-1.jpg" alt="Waveshare ESP32-S3-Touch-LCD-1.69" width="500">

## Repository Structure

This repository provides sample programs, bundled Arduino libraries, factory firmware, and hardware design files for the ESP32-S3-Touch-LCD-1.69.

```
.
├── .github/workflows/     # CI for ESP-IDF and Arduino examples
├── assets/                # Product images used in the README
├── config/                # Shared ESP-IDF configuration overlays
├── docs/                  # CI, firmware, and component notes
├── examples/              # Sample programs
│   ├── Arduino/           # Arduino sketches and bundled libraries
│   └── ESP-IDF/           # ESP-IDF projects
├── firmware/              # Pre-built factory firmware (.bin)
├── hardware/              # Schematics, pinout, and dimension drawings
├── releases/              # CI artifact packaging notes and ignored outputs
└── HARDWARE_REFERENCE.md  # Hardware quick reference
```

## Getting Started

Pre-built firmware is available in [`firmware/`](firmware). For build environments, flashing steps, pin mappings, and configuration, refer to the [documentation page](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.69).

For a structured hardware reference designed for both developers and AI coding assistants — covering onboard peripherals, GPIO assignments, I2C addresses, and expansion interface signals — see [HARDWARE_REFERENCE.md](HARDWARE_REFERENCE.md).

## Continuous Integration

GitHub Actions validates the first-party ESP-IDF and Arduino examples. Bundled-library internal examples are intentionally excluded from product CI. See [docs/ci.md](docs/ci.md), [docs/firmware.md](docs/firmware.md), and [docs/components.md](docs/components.md) for the CI matrix, firmware artifact policy, and component migration notes.

## Contributing

We welcome contributions! Here's how you can help:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Commit your changes with clear descriptions.
4. Submit a pull request for review.

## Issues and Support

Open an [issue](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-1.69/issues) with detailed information, or contact the Waveshare team with your order number for technical support.

## License

Licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

---

Thank you for using Waveshare Electronics Products! 🚀
