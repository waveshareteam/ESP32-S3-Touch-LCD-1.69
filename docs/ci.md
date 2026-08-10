# Continuous Integration

[简体中文](ci_ZH.md)

The examples workflow always runs its lightweight classification and first-party
Markdown checks. It uses a complete, rename-aware base-to-head diff; an empty
or unavailable diff fails classification rather than selecting every build.

`scripts/ci_routing.py` routes each framework as `none`, `selected`, or `all`.
Root Markdown, project Markdown, sketch Markdown, and bundled-library Markdown
select no product builds. Direct first-party example source/config selects the
affected project or sketch; shared Arduino libraries select all Arduino
sketches; config, workflow, discovery, build, or packaging inputs select all
applicable examples. Unknown complete non-document paths are conservatively
reported and select all examples. Renames and deletions consider both paths.

`firmware/` Markdown, source/configuration, binaries, and archives are reported
as firmware/release evidence and remain outside the examples matrix.
Documentation-only and governance-only changes therefore skip expensive builds
by design while the lightweight job remains visible.

The full product matrix has 19 entries: four ESP-IDF projects on `v5.5.5` and
`v6.0.2` (8 builds) plus 11 first-party Arduino sketches on Arduino-ESP32
`3.3.11`. Bundled-library sketches are excluded. `workflow_dispatch` accepts
`all`, a directory name, or a repository-relative example path.

This lightweight gate runs static checks only. Local ESP-IDF and Arduino product
builds are not part of it; hardware behavior still requires board testing.
