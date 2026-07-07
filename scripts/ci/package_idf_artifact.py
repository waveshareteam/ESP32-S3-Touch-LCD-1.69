#!/usr/bin/env python3
"""Package ESP-IDF build outputs into a flashable archive for CI artifacts."""

from __future__ import annotations

import argparse
import json
import re
import shutil
import stat
import zipfile
from pathlib import Path


def safe_name(value: str) -> str:
    value = value.replace("\\", "/").strip("/")
    value = value.replace("/", "-")
    return re.sub(r"[^A-Za-z0-9_.-]+", "-", value).strip("-") or "firmware"


def offset_key(offset: str) -> int:
    try:
        return int(str(offset), 0)
    except ValueError:
        return 0


def load_flash_files(build_dir: Path) -> tuple[dict, list[tuple[str, str]]]:
    flasher_args = build_dir / "flasher_args.json"
    if not flasher_args.is_file():
        raise SystemExit(f"Missing ESP-IDF flasher arguments: {flasher_args}")
    data = json.loads(flasher_args.read_text(encoding="utf-8"))
    raw = data.get("flash_files", {})
    if not isinstance(raw, dict) or not raw:
        raise SystemExit(f"No flash_files found in {flasher_args}")
    return data, sorted(raw.items(), key=lambda item: offset_key(item[0]))


def copy_flash_files(build_dir: Path, stage: Path, flash_files: list[tuple[str, str]]) -> list[dict[str, str]]:
    bin_dir = stage / "bin"
    bin_dir.mkdir(parents=True, exist_ok=True)
    copied: list[dict[str, str]] = []
    used_names: set[str] = set()
    for offset, source_name in flash_files:
        source = Path(source_name)
        if not source.is_absolute():
            source = build_dir / source
        if not source.is_file():
            raise SystemExit(f"Flash file not found: {source}")
        dest_name = source.name
        if dest_name in used_names:
            dest_name = f"{offset_key(offset):x}-{dest_name}"
        used_names.add(dest_name)
        dest = bin_dir / dest_name
        shutil.copy2(source, dest)
        copied.append({"offset": offset, "path": dest.relative_to(stage).as_posix()})
    return copied


def write_flash_helpers(stage: Path, target: str, flash_files: list[dict[str, str]], flash_settings: dict) -> None:
    flash_mode = flash_settings.get("flash_mode", "dio")
    flash_freq = flash_settings.get("flash_freq", "80m")
    flash_size = flash_settings.get("flash_size", "16MB")
    pairs = " ".join(f'{item["offset"]} "{item["path"]}"' for item in flash_files)
    sh = stage / "flash.sh"
    sh.write_text(
        "#!/usr/bin/env sh\n"
        "set -eu\n"
        'PORT="${1:-/dev/ttyUSB0}"\n'
        'BAUD="${BAUD:-921600}"\n'
        'SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"\n'
        'cd "$SCRIPT_DIR"\n'
        f'python -m esptool --chip {target} --port "$PORT" --baud "$BAUD" '
        f"write_flash --flash_mode {flash_mode} --flash_freq {flash_freq} "
        f"--flash_size {flash_size} {pairs}\n",
        encoding="utf-8",
    )
    sh.chmod(sh.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

    bat_pairs = " ".join(f'{item["offset"]} "{item["path"]}"' for item in flash_files)
    (stage / "flash.bat").write_text(
        "@echo off\r\n"
        "set PORT=%1\r\n"
        "if \"%PORT%\"==\"\" set PORT=COM3\r\n"
        "if \"%BAUD%\"==\"\" set BAUD=921600\r\n"
        f"python -m esptool --chip {target} --port %PORT% --baud %BAUD% "
        f"write_flash --flash_mode {flash_mode} --flash_freq {flash_freq} "
        f"--flash_size {flash_size} {bat_pairs}\r\n",
        encoding="utf-8",
    )


def make_zip(stage: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(stage.rglob("*")):
            if path.is_file():
                archive.write(path, path.relative_to(stage).as_posix())


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--project", required=True)
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--idf-version", required=True)
    parser.add_argument("--target", default="esp32s3")
    args = parser.parse_args()

    build_dir = args.build_dir
    output_dir = args.output_dir
    artifact_name = f"{safe_name(args.project)}-esp-idf-{safe_name(args.idf_version)}"
    stage = output_dir / artifact_name
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)

    flasher_data, raw_flash_files = load_flash_files(build_dir)
    flash_files = copy_flash_files(build_dir, stage, raw_flash_files)
    manifest = {
        "type": "esp-idf",
        "project": args.project,
        "idf_version": args.idf_version,
        "target": args.target,
        "flash_settings": flasher_data.get("flash_settings", {}),
        "flash_files": flash_files,
    }
    (stage / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    write_flash_helpers(stage, args.target, flash_files, manifest["flash_settings"])
    make_zip(stage, output_dir / f"{artifact_name}.zip")
    print((output_dir / f"{artifact_name}.zip").as_posix())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
