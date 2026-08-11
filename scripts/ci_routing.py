#!/usr/bin/env python3
"""Fail-closed routing for first-party example CI."""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import PurePosixPath

IDF_ROOT = "examples/esp-idf/"
ARDUINO_ROOT = "examples/arduino/"
BUNDLED_ROOT = "examples/arduino/libraries/"
DOC_SUFFIXES = {".md", ".markdown", ".rst"}
DOC_NAMES = {"readme.txt"}
GLOBAL_PREFIXES = (".github/workflows/", "scripts/", "releases/", "config/")


def norm(path: str) -> str:
    path = path.replace("\\", "/").strip()
    while path.startswith("./"):
        path = path[2:]
    return path


def project_selector(path: str, root: str) -> str | None:
    parts = norm(path).split("/")
    root_parts = root.strip("/").split("/")
    if parts[:len(root_parts)] != root_parts or len(parts) <= len(root_parts):
        return None
    return "/".join(parts[:len(root_parts) + 1])


def parse_status_lines(lines: list[str]) -> list[str]:
    paths: list[str] = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        fields = line.split("\t")
        if len(fields) >= 3 and fields[0][:1] in {"R", "C"}:
            paths.extend(fields[1:3])
        elif len(fields) >= 2:
            paths.append(fields[-1])
        else:
            paths.append(line)
    return [norm(path) for path in paths if norm(path)]


def classify(paths: list[str]) -> dict[str, object]:
    if not paths:
        raise ValueError("changed-file input is empty or unavailable")
    idf: set[str] = set()
    arduino: set[str] = set()
    idf_all = arduino_all = False
    firmware: list[str] = []
    release: list[str] = []
    unknown: list[str] = []
    docs_only = True
    for path in paths:
        suffix = PurePosixPath(path).suffix.lower()
        is_doc = suffix in DOC_SUFFIXES or PurePosixPath(path).name.lower() in DOC_NAMES
        if path.startswith("firmware/"):
            firmware.append(path)
            if suffix in {".bin", ".zip"}:
                release.append(path)
            if not is_doc:
                docs_only = False
            continue
        if is_doc:
            continue
        if path.startswith(BUNDLED_ROOT):
            arduino_all = True
            docs_only = False
            continue
        if path.startswith(IDF_ROOT):
            selector = project_selector(path, IDF_ROOT)
            docs_only = False
            if selector and selector.rsplit("/", 1)[-1] not in {"components", "common", "shared"}:
                idf.add(selector)
            else:
                idf_all = True
            continue
        if path.startswith(ARDUINO_ROOT):
            selector = project_selector(path, ARDUINO_ROOT)
            docs_only = False
            if selector:
                arduino.add(selector)
            else:
                arduino_all = True
            continue
        if path.startswith(GLOBAL_PREFIXES):
            docs_only = False
            idf_all = arduino_all = True
            continue
        docs_only = False
        unknown.append(path)
        idf_all = arduino_all = True

    def route(all_selected: bool, selected: set[str]) -> tuple[str, list[str]]:
        if all_selected:
            return "all", []
        if selected:
            return "selected", sorted(selected)
        return "none", []
    idf_route, idf_selectors = route(idf_all, idf)
    arduino_route, arduino_selectors = route(arduino_all, arduino)
    return {
        "idf_route": idf_route, "idf_selectors": idf_selectors,
        "arduino_route": arduino_route, "arduino_selectors": arduino_selectors,
        "docs_only": docs_only, "firmware_paths": firmware,
        "release_paths": release, "unknown_paths": unknown,
    }


def git_changed(base: str) -> list[str]:
    result = subprocess.run(
        ["git", "diff", "--name-status", "-M", f"{base}...HEAD"],
        text=True, capture_output=True, check=False,
    )
    if result.returncode:
        raise ValueError(result.stderr.strip() or "git diff failed")
    return parse_status_lines(result.stdout.splitlines())


def main() -> int:
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--base")
    group.add_argument("--changed-file", action="append", dest="changed_files")
    group.add_argument("--manual-selector")
    parser.add_argument("--github-output")
    args = parser.parse_args()
    try:
        if args.manual_selector is not None:
            selector = norm(args.manual_selector)
            if not selector:
                raise ValueError("manual selector is empty")
            result = {"idf_route": "all" if selector == "all" else "selected", "idf_selectors": [] if selector == "all" else [selector], "arduino_route": "all" if selector == "all" else "selected", "arduino_selectors": [] if selector == "all" else [selector], "docs_only": False, "firmware_paths": [], "release_paths": [], "unknown_paths": []}
        else:
            paths = git_changed(args.base) if args.base else parse_status_lines(args.changed_files or [])
            result = classify(paths)
    except ValueError as exc:
        print(f"ci_routing: {exc}", file=sys.stderr)
        return 2
    output = json.dumps(result, separators=(",", ":"))
    if args.github_output:
        with open(args.github_output, "a", encoding="utf-8") as stream:
            for key, value in result.items():
                stream.write(f"{key}={json.dumps(value) if isinstance(value, (list, dict)) else str(value).lower() if isinstance(value, bool) else value}\n")
    print(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
