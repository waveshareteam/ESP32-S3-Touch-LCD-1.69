#!/usr/bin/env python3
"""Small, stdlib-only checker for this repository's first-party Markdown.

This is intentionally narrower than the repository modernization Markdown audit.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

PAIRS = (
    ("README.md", "README_ZH.md"),
    ("HARDWARE_REFERENCE.md", "HARDWARE_REFERENCE_ZH.md"),
    ("CONTRIBUTING.md", "CONTRIBUTING_ZH.md"), ("SUPPORT.md", "SUPPORT_ZH.md"),
    ("SECURITY.md", "SECURITY_ZH.md"), ("config/README.md", "config/README_ZH.md"),
    ("docs/ci.md", "docs/ci_ZH.md"), ("docs/components.md", "docs/components_ZH.md"),
    ("docs/firmware.md", "docs/firmware_ZH.md"),
    ("docs/repository-structure.md", "docs/repository-structure_ZH.md"),
    ("releases/README.md", "releases/README_ZH.md"),
    ("examples/esp-idf/02_ESP_IDF_ST7789_LVGL/README.md", "examples/esp-idf/02_ESP_IDF_ST7789_LVGL/README_ZH.md"),
)
LINK = re.compile(r"!?\[[^]]*\]\(([^)\s]+)(?:\s+[^)]*)?\)")
SENSITIVE = (
    ("local absolute path", re.compile(r"(?m)(?:^|[\s(])(?:[A-Za-z]:[\\/]|/(?:Users|home|tmp|var)/)")),
    ("serial port", re.compile(r"\b(?:COM[1-9]\d*|/dev/tty\w+)\b", re.I)),
    ("credential", re.compile(r"\b(?:api[_-]?key|password|token)\s*[:=]\s*[^\s<]+", re.I)),
    ("editor/model provenance", re.compile(r"\b(?:generated|written|edited)\s+(?:by|with)\s+(?:codex|chatgpt|claude|cursor|copilot)\b", re.I)),
)


def text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def headings(content: str) -> set[str]:
    return {re.sub(r"[^a-z0-9 -]", "", line.lstrip("#").strip().lower()).replace(" ", "-") for line in content.splitlines() if line.startswith("#")}


def check(repo: Path) -> list[str]:
    errors: list[str] = []
    paired = {item for pair in PAIRS for item in pair}
    companion = {a: b for a, b in PAIRS} | {b: a for a, b in PAIRS}
    for english, chinese in PAIRS:
        for source, other in ((english, chinese), (chinese, english)):
            path = repo / source
            if not path.is_file():
                errors.append(f"missing first-party document: {source}")
                continue
            content = text(path)
            if Path(other).name not in content[:1200]:
                errors.append(f"missing reciprocal language link: {source} -> {other}")
            for label, rule in SENSITIVE:
                if rule.search(content):
                    errors.append(f"{label}: {source}")
                    break
            for raw in LINK.findall(content):
                target = raw.split("#", 1)[0].replace("%20", " ")
                fragment = raw.partition("#")[2]
                if not target or "://" in target or target.startswith("mailto:"):
                    continue
                destination = (path.parent / target).resolve()
                try:
                    destination.relative_to(repo.resolve())
                except ValueError:
                    errors.append(f"repository-escaping link: {source} -> {raw}")
                    continue
                if not destination.exists():
                    errors.append(f"missing relative target: {source} -> {raw}")
                    continue
                if fragment and destination.is_file() and fragment not in headings(text(destination)):
                    errors.append(f"missing fragment: {source} -> {raw}")
                if source in paired and destination.is_file():
                    rel = destination.relative_to(repo).as_posix()
                    if rel in companion and rel != companion[source] and rel.endswith("_ZH.md") != source.endswith("_ZH.md"):
                        errors.append(f"wrong-language link: {source} -> {rel}")
    en = text(repo / "README.md") if (repo / "README.md").is_file() else ""
    zh = text(repo / "README_ZH.md") if (repo / "README_ZH.md").is_file() else ""
    for marker in ("🌐", "📚", "📦", "🧩", "🔧"):
        if (marker in en) != (marker in zh):
            errors.append(f"homepage quick-link mismatch: {marker}")
    en_h2 = [line.split()[1] if len(line.split()) > 1 else "" for line in en.splitlines() if line.startswith("## ")]
    zh_h2 = [line.split()[1] if len(line.split()) > 1 else "" for line in zh.splitlines() if line.startswith("## ")]
    if en_h2 != zh_h2:
        errors.append("homepage H2 emoji sequence mismatch")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("repo", nargs="?", default=".")
    args = parser.parse_args()
    errors = check(Path(args.repo).resolve())
    for error in errors:
        print(error)
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
