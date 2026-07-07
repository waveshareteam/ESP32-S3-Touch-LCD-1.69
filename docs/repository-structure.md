# Repository Structure

This repository uses the canonical Waveshare ESP32 product layout for maintained examples and release helpers:

- `examples/esp-idf/`: first-party ESP-IDF projects for the ESP32-S3 Touch LCD 1.69 board.
- `examples/arduino/`: first-party Arduino sketches.
- `examples/arduino/libraries/`: bundled Arduino libraries required by those sketches.
- `config/`: shared configuration overlays used by more than one example, when present.
- `docs/`: maintainer notes for CI, components, firmware, and repository structure.
- `firmware/`: factory binary artifacts that are documented but not built in CI.
- `releases/`: scripts for packaging build outputs into flashable firmware archives and downloading CI artifacts.
- `assets/`: product images used by documentation.
- `schematic/`: public schematic files.

CI intentionally builds only first-party examples. Examples and tests inside bundled Arduino libraries remain available for library users, but they are not product CI targets.
