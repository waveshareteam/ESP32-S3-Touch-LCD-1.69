#!/usr/bin/env python3
"""Limited, stdlib-only checks for this repository's first-party Markdown.

This helper deliberately does not replace the complete repository-modernization
Markdown audit. It checks the configured homepage contract plus the local
first-party pairing, link, and public-text hygiene rules used by this workflow.
"""
from __future__ import annotations

import argparse
import fnmatch
import json
import re
import sys
import unicodedata
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
HTML_TAG = re.compile(r"<[^>]+>")
H2 = re.compile(r"^##\s+([^\s]+)", re.M)
H3 = re.compile(r"^###\s+(.+)$", re.M)
SENSITIVE = (
    ("local absolute path", re.compile(r"(?m)(?:^|[\s(])(?:[A-Za-z]:[\\/]|/(?:Users|home|tmp|var)/)")),
    # Generic flashing placeholders are documentation, not device identifiers.
    ("serial port", re.compile(r"\b(?:COM(?!x\b)[1-9]\d*|/dev/tty(?!ACM0\b)\w+)\b", re.I)),
    ("credential", re.compile(r"\b(?:api[_-]?key|password|token)\s*[:=]\s*[^\s<]+", re.I)),
    ("editor/model provenance", re.compile(r"\b(?:generated|written|edited)\s+(?:by|with)\s+(?:codex|chatgpt|claude|cursor|copilot)\b", re.I)),
)
CONFIG_KEYS = {"pair_exempt_patterns", "homepage_pairs"}
HOMEPAGE_KEYS = {"english", "chinese", "profile", "required_components", "required_quick_links", "required_badges", "required_h2_icons", "h3_emoji_allow_patterns"}
COMPONENTS = {"centered_header", "html_h1", "subtitle", "badges", "language_switch", "quick_links", "hero_image", "separator", "h2"}
QUICK_LINKS = {"product", "documentation", "firmware", "quick_start", "esp_idf", "arduino"}
BADGES = {"build", "release", "license"}
PROFILES = {"single-product", "multi-product-hub", "auto"}


class ConfigError(ValueError):
    """A policy file cannot be safely interpreted."""


def text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def headings(content: str) -> set[str]:
    return {re.sub(r"[^a-z0-9 -]", "", line.lstrip("#").strip().lower()).replace(" ", "-") for line in content.splitlines() if line.startswith("#")}


def _string_list(value: object, name: str) -> list[str]:
    if not isinstance(value, list) or not all(isinstance(item, str) and item for item in value):
        raise ConfigError(f"{name} must be a list of non-empty strings")
    return value


def load_config(path: Path | None) -> dict[str, object]:
    config: dict[str, object] = {"pair_exempt_patterns": [], "homepage_pairs": []}
    if path is None:
        return config
    try:
        loaded = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ConfigError(f"cannot read JSON config {path}: {exc}") from exc
    if not isinstance(loaded, dict):
        raise ConfigError("config root must be a JSON object")
    unknown = sorted(set(loaded) - CONFIG_KEYS)
    if unknown:
        raise ConfigError("unknown config keys: " + ", ".join(unknown))
    if "pair_exempt_patterns" in loaded:
        config["pair_exempt_patterns"] = _string_list(loaded["pair_exempt_patterns"], "pair_exempt_patterns")
    if "homepage_pairs" in loaded:
        config["homepage_pairs"] = loaded["homepage_pairs"]
    pairs = config["homepage_pairs"]
    if not isinstance(pairs, list):
        raise ConfigError("homepage_pairs must be a list")
    for index, pair in enumerate(pairs):
        prefix = f"homepage_pairs[{index}]"
        if not isinstance(pair, dict):
            raise ConfigError(f"{prefix} must be an object")
        unknown_pair = sorted(set(pair) - HOMEPAGE_KEYS)
        missing_pair = sorted(HOMEPAGE_KEYS - set(pair))
        if unknown_pair or missing_pair:
            details = []
            if unknown_pair:
                details.append("unknown keys: " + ", ".join(unknown_pair))
            if missing_pair:
                details.append("missing keys: " + ", ".join(missing_pair))
            raise ConfigError(f"{prefix} " + "; ".join(details))
        for key in ("english", "chinese", "profile"):
            if not isinstance(pair[key], str) or not pair[key]:
                raise ConfigError(f"{prefix}.{key} must be a non-empty string")
        if pair["profile"] not in PROFILES:
            raise ConfigError(f"{prefix}.profile is unsupported: {pair['profile']}")
        required_components = _string_list(pair["required_components"], f"{prefix}.required_components")
        required_quick_links = _string_list(pair["required_quick_links"], f"{prefix}.required_quick_links")
        required_badges = _string_list(pair["required_badges"], f"{prefix}.required_badges")
        _string_list(pair["required_h2_icons"], f"{prefix}.required_h2_icons")
        patterns = _string_list(pair["h3_emoji_allow_patterns"], f"{prefix}.h3_emoji_allow_patterns")
        if set(required_components) - COMPONENTS or set(required_quick_links) - QUICK_LINKS or set(required_badges) - BADGES:
            raise ConfigError(f"{prefix} contains unsupported contract keys")
        for pattern in patterns:
            try:
                if not pattern.startswith("^") or not pattern.endswith("$") or re.compile(pattern).match(""):
                    raise ConfigError(f"{prefix}.h3_emoji_allow_patterns must be anchored and non-empty")
            except re.error as exc:
                raise ConfigError(f"{prefix}.h3_emoji_allow_patterns has invalid regex: {exc}") from exc
    return config


def _local_target(repo: Path, source: Path, raw: str) -> tuple[Path | None, str | None]:
    target, _, fragment = raw.partition("#")
    target = target.replace("%20", " ")
    if not target:
        return source, fragment
    if "://" in target or target.startswith(("mailto:", "data:")):
        return None, fragment
    destination = (source.parent / target).resolve()
    try:
        destination.relative_to(repo.resolve())
    except ValueError:
        return None, "__ESCAPES__"
    return destination, fragment


def _html_values(content: str, attribute: str) -> list[str]:
    values: list[str] = []
    for tag in HTML_TAG.findall(content):
        for name, _, value in re.findall(r"\b(href|src)\s*=\s*([\"'])(.*?)\2", tag, re.I | re.S):
            if name.lower() == attribute:
                values.append(value)
    return values


def _html_local_paths(repo: Path, source: Path, content: str) -> list[str]:
    errors: list[str] = []
    for raw in _html_values(content, "href") + _html_values(content, "src"):
        destination, fragment = _local_target(repo, source, raw)
        if fragment == "__ESCAPES__":
            errors.append(f"repository-escaping HTML path: {source.relative_to(repo)} -> {raw}")
        elif destination is not None and not destination.exists():
            errors.append(f"missing local HTML path: {source.relative_to(repo)} -> {raw}")
    return errors


def _has_emoji(value: str) -> bool:
    return any(unicodedata.category(char) == "So" for char in value)


def _quick_link_key(href: str) -> str | None:
    value = href.lower()
    if "docs.waveshare" in value:
        return "documentation"
    if "waveshare.com/" in value or "waveshare.net/" in value:
        return "product"
    if "quick-start" in value or "quick_start" in value:
        return "quick_start"
    if "/releases" in value or value.startswith("releases/"):
        return "firmware"
    if "examples/esp-idf" in value:
        return "esp_idf"
    if "examples/arduino" in value:
        return "arduino"
    return None


def _homepage_errors(repo: Path, pair: dict[str, object]) -> list[str]:
    errors: list[str] = []
    english, chinese, profile = (str(pair[key]) for key in ("english", "chinese", "profile"))
    components = set(pair["required_components"])
    quick_links, badges = set(pair["required_quick_links"]), set(pair["required_badges"])
    expected_h2 = pair["required_h2_icons"]
    h3_patterns = [re.compile(pattern) for pattern in pair["h3_emoji_allow_patterns"]]
    contents: dict[str, str] = {}
    for path_text in (english, chinese):
        path = repo / path_text
        if not path.is_file():
            errors.append(f"homepage {profile} missing: {path_text}")
        else:
            contents[path_text] = text(path)
    if len(contents) != 2:
        return errors
    h2_sequences: dict[str, list[str]] = {}
    for path_text, content in contents.items():
        path = repo / path_text
        errors.extend(_html_local_paths(repo, path, content))
        h2_sequences[path_text] = H2.findall(content)
        if "centered_header" in components and not re.search(r"<div\s+align=[\"']center[\"']\s*>", content, re.I):
            errors.append(f"homepage missing centered_header: {path_text}")
        if "html_h1" in components and not re.search(r"<h1>[^<]+</h1>", content, re.I):
            errors.append(f"homepage missing html_h1: {path_text}")
        if "subtitle" in components and not re.search(r"<p>\s*<strong>[^<]+</strong>\s*</p>", content, re.I):
            errors.append(f"homepage missing subtitle: {path_text}")
        if "separator" in components and not re.search(r"(?m)^---\s*$", content):
            errors.append(f"homepage missing separator: {path_text}")
        if "h2" in components and not h2_sequences[path_text]:
            errors.append(f"homepage missing h2: {path_text}")
        hrefs, srcs = _html_values(content, "href"), _html_values(content, "src")
        if "language_switch" in components and (chinese if path_text == english else english) not in hrefs:
            errors.append(f"homepage missing language_switch: {path_text}")
        found_links = {_quick_link_key(href) for href in hrefs}
        for key in quick_links:
            if key not in found_links:
                errors.append(f"homepage missing quick link {key}: {path_text}")
        local_images = []
        for src in srcs:
            destination, fragment = _local_target(repo, path, src)
            if destination is not None and destination.suffix.lower() in {".gif", ".jpeg", ".jpg", ".png", ".svg", ".webp"}:
                local_images.append((src, destination, fragment))
        if "hero_image" in components:
            if not local_images:
                errors.append(f"homepage missing local hero_image: {path_text}")
            for src, destination, fragment in local_images:
                if fragment == "__ESCAPES__" or not destination.exists():
                    errors.append(f"homepage invalid local hero_image: {path_text} -> {src}")
        if "badges" in components:
            joined = " ".join(srcs + hrefs).lower()
            matches = {"build": "actions/workflows" in joined or "badge.svg" in joined, "release": "/releases/latest" in joined or "github/v/release" in joined, "license": "license" in joined}
            for key in badges:
                if not matches[key]:
                    errors.append(f"homepage missing badge {key}: {path_text}")
        for heading in H3.findall(content):
            if _has_emoji(heading) and not any(pattern.search(heading) for pattern in h3_patterns):
                errors.append(f"homepage disallowed H3 emoji: {path_text} -> {heading}")
    if h2_sequences[english] != list(expected_h2) or h2_sequences[chinese] != list(expected_h2):
        errors.append(f"homepage H2 icon contract mismatch: {english}, {chinese}")
    return errors


def check(repo: Path, config: Path | None = None) -> list[str]:
    try:
        policy = load_config(config)
    except ConfigError as exc:
        return [f"invalid markdown audit config: {exc}"]
    errors: list[str] = []
    exemptions = policy["pair_exempt_patterns"]
    active_pairs = tuple(pair for pair in PAIRS if not any(fnmatch.fnmatch(item, pattern) for item in pair for pattern in exemptions))
    paired = {item for pair in active_pairs for item in pair}
    companion = {a: b for a, b in active_pairs} | {b: a for a, b in active_pairs}
    for english, chinese in active_pairs:
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
                destination, fragment = _local_target(repo, path, raw)
                if fragment == "__ESCAPES__":
                    errors.append(f"repository-escaping link: {source} -> {raw}")
                    continue
                if destination is None:
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
            errors.extend(_html_local_paths(repo, path, content))
    for homepage_pair in policy["homepage_pairs"]:
        errors.extend(_homepage_errors(repo, homepage_pair))
    return list(dict.fromkeys(errors))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("repo", nargs="?", default=".")
    parser.add_argument("--config", type=Path, help="homepage-contract JSON policy")
    args = parser.parse_args()
    errors = check(Path(args.repo).resolve(), args.config)
    for error in errors:
        print(error)
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
