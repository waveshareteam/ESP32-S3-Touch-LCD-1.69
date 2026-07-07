#!/usr/bin/env python3
"""Discover first-party ESP-IDF projects or Arduino sketches for CI.

The repository currently keeps historical example roots with capitalized names.
This script accepts those roots and the canonical lowercase roots so workflows can
be stable during a future layout migration.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


IDF_ROOTS = (Path("examples/esp-idf"), Path("examples/ESP-IDF"))
ARDUINO_ROOTS = (
    Path("examples/arduino/examples"),
    Path("examples/Arduino/examples"),
)


def actual_path(path: Path) -> Path:
    current = Path(".")
    for part in path.parts:
        if not current.is_dir():
            return path
        match = next((child for child in current.iterdir() if child.name.lower() == part.lower()), None)
        if match is None:
            return path
        current = match
    return current


def rel(path: Path) -> str:
    return path.as_posix()


def dedupe(items: list[dict[str, str]]) -> list[dict[str, str]]:
    seen: set[str] = set()
    result: list[dict[str, str]] = []
    for item in items:
        key = item["path"].lower()
        if key in seen:
            continue
        seen.add(key)
        result.append(item)
    return result


def discover_idf() -> list[dict[str, str]]:
    examples: list[dict[str, str]] = []
    for candidate in IDF_ROOTS:
        root = actual_path(candidate)
        if not root.is_dir():
            continue
        for child in sorted(root.iterdir(), key=lambda p: p.name.lower()):
            if not child.is_dir():
                continue
            if not (child / "CMakeLists.txt").is_file():
                continue
            examples.append({"name": child.name, "path": rel(child)})
    return dedupe(examples)


def discover_arduino() -> list[dict[str, str]]:
    sketches: list[dict[str, str]] = []
    for candidate in ARDUINO_ROOTS:
        root = actual_path(candidate)
        if not root.is_dir():
            continue
        for child in sorted(root.iterdir(), key=lambda p: p.name.lower()):
            if not child.is_dir():
                continue
            if any(part.lower() == "libraries" for part in child.parts):
                continue
            ino_files = sorted(child.glob("*.ino"), key=lambda p: p.name.lower())
            if not ino_files:
                continue
            sketches.append({"name": child.name, "path": rel(ino_files[0])})
    return dedupe(sketches)


def matches(item: dict[str, str], selector: str) -> bool:
    selector = selector.strip().replace("\\", "/").strip("/")
    if not selector or selector.lower() == "all":
        return True
    selector_lower = selector.lower()
    name_lower = item["name"].lower()
    path_lower = item["path"].lower()
    return (
        selector_lower == name_lower
        or selector_lower == path_lower
        or path_lower.endswith("/" + selector_lower)
        or path_lower.startswith(selector_lower + "/")
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("kind", choices=("idf", "arduino"))
    parser.add_argument("--filter", default="all", help="all, an example name, or a repo-relative path")
    args = parser.parse_args()

    items = discover_idf() if args.kind == "idf" else discover_arduino()
    selected = [item for item in items if matches(item, args.filter)]
    if not selected:
        raise SystemExit(f"No {args.kind} examples matched filter: {args.filter}")
    print(json.dumps({"include": selected}, separators=(",", ":")))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
