#!/usr/bin/env python3
"""Discover first-party examples for CI."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


IDF_ROOT = Path("examples/esp-idf")
ARDUINO_ROOT = Path("examples/arduino")
DEFAULT_ARDUINO_FQBN = "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi"


def normalize(value: str) -> str:
    return value.replace("\\", "/").strip("/")


def selector_matches(entry: dict[str, str], selectors: list[str]) -> bool:
    if not selectors or "all" in selectors:
        return True
    path = normalize(entry["path"])
    name = entry["name"]
    for selector in selectors:
        selector = normalize(selector)
        if (
            selector == name
            or selector == path
            or path.startswith(selector + "/")
            or selector in path.split("/")
        ):
            return True
    return False


def dedupe(entries: list[dict[str, str]]) -> list[dict[str, str]]:
    seen: set[str] = set()
    result: list[dict[str, str]] = []
    for entry in entries:
        key = normalize(entry["path"]).lower()
        if key in seen:
            continue
        seen.add(key)
        result.append(entry)
    return result


def discover_esp_idf(repo: Path) -> list[dict[str, str]]:
    entries: list[dict[str, str]] = []
    path = repo / IDF_ROOT
    if not path.exists():
        return entries
    for project in sorted(path.iterdir(), key=lambda item: item.name.lower()):
        if project.is_dir() and (project / "CMakeLists.txt").exists():
            entries.append({"name": project.name, "path": project.relative_to(repo).as_posix()})
    return dedupe(entries)


def discover_arduino(repo: Path) -> list[dict[str, str]]:
    entries: list[dict[str, str]] = []
    path = repo / ARDUINO_ROOT
    if not path.exists():
        return entries
    for ino in sorted(path.rglob("*.ino"), key=lambda item: item.as_posix().lower()):
        rel = ino.relative_to(repo).as_posix()
        if rel.lower().startswith("examples/arduino/libraries/"):
            continue
        sketch_dir = ino.parent.relative_to(repo).as_posix()
        entries.append({"name": ino.parent.name, "path": sketch_dir})
    return dedupe(entries)


def build_matrix(args: argparse.Namespace) -> dict[str, list[dict[str, str]]]:
    repo = Path(args.repo).resolve()
    selectors = [normalize(item) for item in args.selector if normalize(item)]
    if args.surface == "esp-idf":
        projects = [entry for entry in discover_esp_idf(repo) if selector_matches(entry, selectors)]
        versions = [item.strip() for item in args.idf_versions.split(",") if item.strip()]
        include = [entry | {"idf": version} for entry in projects for version in versions]
    else:
        sketches = [entry for entry in discover_arduino(repo) if selector_matches(entry, selectors)]
        include = [entry | {"core": args.arduino_core, "fqbn": args.fqbn} for entry in sketches]
    return {"include": include}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=".")
    parser.add_argument("--surface", choices=("esp-idf", "arduino"), required=True)
    parser.add_argument("--selector", action="append", default=[])
    parser.add_argument("--idf-versions", default="v5.5.5,v6.0.2")
    parser.add_argument("--arduino-core", default="3.3.11")
    parser.add_argument("--fqbn", default=DEFAULT_ARDUINO_FQBN)
    parser.add_argument("--github-output")
    args = parser.parse_args()

    matrix = build_matrix(args)
    output = json.dumps(matrix, separators=(",", ":"))
    count = len(matrix["include"])
    if args.github_output:
        with open(args.github_output, "a", encoding="utf-8") as fh:
            fh.write(f"matrix={output}\n")
            fh.write(f"count={count}\n")
    else:
        print(output)


if __name__ == "__main__":
    main()
