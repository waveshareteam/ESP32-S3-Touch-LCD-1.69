# Contributing

[简体中文](CONTRIBUTING_ZH.md)

Thanks for improving this repository.

## Workflow

1. Open an issue for larger behavior changes when the direction is not already clear.
2. Keep changes scoped to the affected example, documentation, workflow, or support file.
3. Use repo-relative paths in reports and pull requests.
4. Submit a pull request and use GitHub Actions as the validation record.

Include the hardware revision, affected example and framework version, and the
validation matrix. State any managed-component, pin/BSP, or factory-firmware
impact and list remaining TODOs. Source-built CI artifacts and factory recovery
firmware are different delivery surfaces.

## Examples And Libraries

- Product ESP-IDF examples live under `examples/esp-idf/`.
- Product Arduino sketches live under `examples/arduino/`.
- Bundled Arduino libraries live under `examples/arduino/libraries/`.
- Bundled-library internal examples are not part of product CI unless a change explicitly targets library-level validation.

## Firmware

Factory binaries under `firmware/` are released recovery artifacts. Do not replace or regenerate them from CI output.
