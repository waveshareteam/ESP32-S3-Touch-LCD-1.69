# Pocket Pet prototype (V2.1 board, Arduino IDE)

Sketch: `pocket_pet/` (open `pocket_pet.ino`; the other files appear as tabs).

## Setup
- Board: **ESP32S3 Dev Module**, esp32 core 3.3.x (repo pins 3.3.11), Flash 16MB, PSRAM **OPI PSRAM** (the full-frame canvas lives in PSRAM).
- USB CDC On Boot: Enabled or Disabled both work.
- Library (only one): Library Manager -> **GFX Library for Arduino** v1.6.5 (the version the repo examples bundle). `pin_config.h` is already in the sketch folder. Touch, RTC, AHT10, VEML7700 and DRV2605L use plain `Wire`, so no other libraries.
- Not on V2.1? Change the four pin defines in `config.h` (V1: buzzer 33, SYS_EN 35, SYS_OUT 36, RTC_INT 41).
- Can't upload? Hold BOOT, tap RESET, release BOOT (see `docs/pocket-pet-design.md` §10).

## Modules
| File | Role |
| --- | --- |
| `board.*` | bring-up (buzzer LOW, SYS_EN HIGH, single `Wire.begin(11,10)`), I2C helpers, CST816T touch, PCF85063 RTC, battery ADC |
| `pet.*` | hunger/happiness/health model, temperature gain/drain, NVS persistence, offline catch-up from RTC |
| `sensors.*` | AHT10 + VEML7700 polling (settings toggle), shutdown between reads |
| `haptics.*` | DRV2605L in LRA mode, library 6, auto-calibration cached in NVS |
| `sound.*` | passive buzzer emotes via LEDC (volume, quiet hours 22-07, pin LOW when idle) |
| `sprite.*` | pet art authored at 32x32, drawn at 4x (128x128) |
| `game.*` | Star Catch minigame |
| `ui.*` | canvas, pet / settings / game screens, temperature effects, backlight |
| `power.*` | ST7789 SLPIN + light sleep with slow sensor wake, low-battery guard |

## Behaviour
- **External boards** (AHT10 0x38, VEML7700 0x10, DRV2605L 0x5A) are probed on boot (shown on the boot screen) and **re-scanned every 10 s and on every sleep wake**, so you can attach them later. Absent boards are simply skipped.
- Cold (<16 C): faster hunger and snow. Hot (>28 C): grumpier and heat shimmer. 20-26 C: slow regen when fed and happy. No sensor = assumes 22 C.
- Feed / Play (Star Catch) / Menu (temp and light sensors toggled separately, sound volume, haptics, auto brightness with manual brightness steps when auto is off, sleep poll 1/5/15/30 min, quiet hours). Tap the pet to pet it.
- Screen turns off after 30 s idle, then light sleep; touch or BOOT wakes, a timer wakes every `sleepPollS` to poll sensors and update the pet at half rate.
- Time: build time seeds the RTC on first boot; there is no UI to set the clock yet. The RTC only survives while the power latch / battery stays on.
- Safety: below ~3.3 V the sketch saves and drops SYS_EN (the board has no battery cutoff). Charge current is set by resistor R25 (2 A, 0.8C for 2500 mAh); see design doc §2.3 before using a cell.

- **Hatching:** a new pet (first boot, or a new egg after death) starts as an egg that wobbles, cracks and hatches over 60 s (RTC-based, so it survives sleep/reboot). Stats are frozen, Feed/Play are disabled and the screen stays on until it hatches; tapping the egg gives a tick. Debug 'Warp' shortens the timer.

## Debug menu
`POCKET_PET_DEBUG` (top of `config.h`, default 1) adds Settings > **Debug menu**: tap the left/right half of a row to change Food / Fun / Health by 10, warp the simulation +1 h / +6 h, override the temperature (sensor, 0, 10, 15, 22, 30, 36 C), kill / revive the pet, and test sounds and haptic effects. Set it to 0 (or build with `-DPOCKET_PET_DEBUG=0`) to compile it out. The prototype has no pet traits yet, so only stats are editable.

## Unverified on hardware (compiled only)
Touch coordinates/orientation, canvas performance in PSRAM, light-sleep wake via TP_INT/BOOT, LRA constants (`LRA_RATED_V_REG`, `LRA_OD_CLAMP_REG`, `LRA_DRIVE_TIME` in `haptics.cpp`: set from your motor datasheet), auto-calibration, buzzer loudness/passive vs active, sensor readings, SYS_EN latch behaviour, battery ADC scale, sleep current.
