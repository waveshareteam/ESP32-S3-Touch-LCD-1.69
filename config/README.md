# Configuration

[简体中文](README_ZH.md)

This directory is reserved for shared ESP-IDF configuration overlays used by CI or multiple examples.

Current examples keep their own `sdkconfig.defaults` or `sdkconfig.ci` files. Add shared overlays here only when they are used by more than one first-party project.

`markdown-audit.json` defines the limited checker’s bilingual homepage contract,
including required header components, links, badges, and section icons.
`ci-routing-audit.json` extends the maintainer routing audit with this
repository’s documentation assets, non-build paths, and global build inputs.
