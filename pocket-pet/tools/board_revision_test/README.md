# Board revision test (V1 vs V2.1)

Arduino sketch that shows on the LCD and over Serial (115200) which revision of the
ESP32-S3-Touch-LCD-1.69 you have.

## How it works (read-only)
1. I2C scan on SDA=11 / SCL=10 (RTC 0x51, touch 0x15, IMU 0x6B, plus optional VEML7700 0x10, AHT10 0x38, DRV2605L 0x5A).
2. Arms a 2 s PCF85063 countdown timer and watches GPIO39 and GPIO41 **as inputs only**.
   The RTC INT line is on GPIO41 (V1) or GPIO39 (V2.1); the one that goes low is the revision.
3. Restores the RTC timer registers, then idles. No sleep, no output on GPIO33/35/36/40/41/42, GPIO0 untouched, so you can always re-flash.

Result: `V2.1` / `V1` (confidence high) or `INCONCLUSIVE` with things to check.

## Arduino IDE settings
- Board: ESP32S3 Dev Module (esp32 core 3.3.x; repo pins 3.3.11)
- Flash size 16MB, PSRAM OPI PSRAM
- USB CDC On Boot: either (sketch handles both)
- Library (only one needed): Library Manager -> install **GFX Library for Arduino** by moselle (v1.6.5, the version the repo uses). `pin_config.h` is already in this sketch folder, so no sketchbook change is needed.
  (Alternative: copy `examples/arduino/libraries/GFX_Library_for_Arduino` into `~/Documents/Arduino/libraries/`.)

## Flashing
If the port is missing: hold BOOT, tap RESET (or plug USB while holding BOOT), release BOOT. See §10 of `docs/pocket-pet-design.md`.

## Status
Compiled with arduino-cli (core 3.3.12) on a clean library setup with only GFX Library for Arduino 1.6.5 installed. Not yet run on hardware; the RTC timer behaviour is unverified.
