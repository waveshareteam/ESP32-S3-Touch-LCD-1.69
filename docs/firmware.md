# Firmware

The `firmware/` directory contains a pre-built factory image for flashing and recovery. The factory firmware source is not part of this repository, so factory binaries are documented artifacts rather than build CI inputs.

Source-built CI firmware comes only from maintained examples:

- ESP-IDF projects under `examples/ESP-IDF/` or `examples/esp-idf/`
- Arduino sketches under `examples/Arduino/examples/` or `examples/arduino/examples/`

CI uploads successful source-built outputs as workflow artifacts. The packaged archive contains a manifest, binaries, and simple flash helpers. These generated archives belong in CI artifact storage, not in source control.

Use the factory binary when you need to restore the board to the released factory state. Use CI-generated artifacts when validating a specific example build from this repository.
