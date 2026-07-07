# Firmware Artifacts

`firmware/` contains the factory binary artifact for user flashing and recovery flows. This binary is not a source project and is not built by CI.

Source-maintained firmware lives under the example trees and is built by `.github/workflows/examples.yml`:

- ESP-IDF projects under `examples/esp-idf/`.
- Arduino sketches under `examples/arduino/`.

CI build outputs are packaged by `releases/package_firmware.py` and uploaded as workflow artifacts. The generated zip contains `manifest.json`, flash helper scripts, flash arguments, and the binaries needed by esptool.

Use the factory binary when restoring the board to the released factory state. Use CI-generated artifacts when validating a specific example build from this repository.

For local release packaging, build the target project first and run the Python script from the repository root. Generated archives are written under `releases/dist/` by default.
