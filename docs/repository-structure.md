# Repository Structure

[简体中文](repository-structure_ZH.md)

- `examples/esp-idf/`: first-party ESP-IDF projects.
- `examples/arduino/`: first-party Arduino sketches; `libraries/` is bundled upstream code.
- `config/`: shared example configuration overlays when used.
- `docs/`: maintainer documentation.
- `firmware/`: immutable factory binary artifacts, outside example CI.
- `releases/`: artifact packaging and download helpers.
- `hardware/schematics/`: public schematic files.
- `assets/`: product images used by documentation.

CI builds only first-party examples, never bundled-library examples or factory firmware.
