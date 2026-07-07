# Continuous Integration

This repository validates source examples in GitHub Actions. Local builds are optional; CI is the validation source for repository changes.

## Inventory Summary

Resolved on 2026-07-07 from upstream primary sources:

- ESP-IDF stable matrix: `v5.5.4` and `v6.0.2`
- Arduino-ESP32 stable core: `3.3.10`
- Target: `esp32s3`
- First-party ESP-IDF projects: direct children of `examples/ESP-IDF/` or `examples/esp-idf/`
- First-party Arduino sketches: direct children of `examples/Arduino/examples/` or `examples/arduino/examples/`
- Excluded from product CI: examples under bundled libraries such as `examples/Arduino/libraries/*/examples`
- Factory binary handling: files under `firmware/` are documented recovery artifacts and are not rebuilt by CI

The current repository shape is a legacy product layout with capitalized example roots. The CI discovery scripts support both the current paths and canonical lowercase paths so a future layout migration can be made without rewriting workflow logic.

## Workflows

- `ESP-IDF examples` builds each first-party ESP-IDF project against `v5.5.4` and `v6.0.2` for `esp32s3`.
- `Arduino examples` compiles each first-party Arduino sketch against Arduino-ESP32 `3.3.10`.
- Both workflows support `workflow_dispatch`.
- Dispatch inputs accept `all`, an example or sketch directory name, or a repo-relative path.

## Firmware Artifacts

CI packages successful builds into archives under workflow artifacts. Each archive includes:

- `manifest.json`
- firmware binaries under `bin/`
- `flash.sh`
- `flash.bat`

Generated archives are workflow outputs only. Do not commit files from `releases/dist/`.

## Maintenance Notes

- Keep CI focused on product examples, not bundled-library example suites.
- If a v6 failure appears, check the official ESP-IDF `5.5 -> 6.0` migration guide for the failing subsystem before changing source.
- If Arduino examples require board options beyond the generic `esp32:esp32:esp32s3` FQBN, adjust `ARDUINO_FQBN` in `.github/workflows/arduino.yml` and let CI validate the change.
