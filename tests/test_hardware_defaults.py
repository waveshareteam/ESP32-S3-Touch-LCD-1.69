import unittest
from pathlib import Path

from scripts.discover_examples import DEFAULT_ARDUINO_FQBN


ROOT = Path(__file__).resolve().parents[1]


class HardwareDefaultsTests(unittest.TestCase):
    def test_project01_board_defaults(self):
        sdkconfig = (ROOT / "examples/esp-idf/01_ESP_IDF_ST7789/sdkconfig.defaults").read_text(
            encoding="utf-8"
        )
        kconfig = (ROOT / "examples/esp-idf/01_ESP_IDF_ST7789/components/st7789/Kconfig.projbuild").read_text(
            encoding="utf-8"
        )
        expected = {
            "WIDTH": 240,
            "HEIGHT": 280,
            "MOSI_GPIO": 7,
            "SCLK_GPIO": 6,
            "CS_GPIO": 5,
            "DC_GPIO": 4,
            "RESET_GPIO": 8,
            "BL_GPIO": 15,
        }
        for name, value in expected.items():
            self.assertIn(f"CONFIG_{name}={value}", sdkconfig)
            self.assertIn(
                f"config {name}\n\t\tint", kconfig,
            )
            self.assertIn(f"default {value} if IDF_TARGET_ESP32S3", kconfig)

    def test_project02_flash_and_psram_profile(self):
        sdkconfig = (ROOT / "examples/esp-idf/02_ESP_IDF_ST7789_LVGL/sdkconfig.defaults").read_text(
            encoding="utf-8"
        )
        for setting in (
            "CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y",
            "CONFIG_SPIRAM=y",
            "CONFIG_SPIRAM_MODE_OCT=y",
            "CONFIG_SPIRAM_SPEED_80M=y",
        ):
            self.assertIn(setting, sdkconfig)
        self.assertNotIn("CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y", sdkconfig)

    def test_arduino_fqbn_uses_board_flash_and_psram(self):
        self.assertIn("FlashSize=16M", DEFAULT_ARDUINO_FQBN)
        self.assertIn("PSRAM=opi", DEFAULT_ARDUINO_FQBN)
