#!/usr/bin/env python3
"""Package Arduino exported binaries into a flashable archive when possible."""

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


def copy_outputs(build_dir: Path, stage: Path) -> tuple[list[dict[str, str]], Path | None]:
    bin_dir = stage / "bin"
    bin_dir.mkdir(parents=True, exist_ok=True)
    outputs = sorted(
        [path for path in build_dir.rglob("*") if path.suffix.lower() in {".bin", ".elf", ".map"}],
        key=lambda p: p.as_posix().lower(),
    )
    if not outputs:
        raise SystemExit(f"No Arduino exported binaries found under {build_dir}")

    copied: list[dict[str, str]] = []
    merged: Path | None = None
    used_names: set[str] = set()
    for source in outputs:
        dest_name = source.name
        if dest_name in used_names:
            dest_name = safe_name(source.relative_to(build_dir).as_posix())
        used_names.add(dest_name)
        dest = bin_dir / dest_name
        shutil.copy2(source, dest)
        rel_dest = dest.relative_to(stage).as_posix()
        copied.append({"path": rel_dest})
        if source.suffix.lower() == ".bin" and "merged" in source.name.lower():
            merged = dest
    return copied, merged


def write_flash_helpers(stage: Path, target: str, merged: Path | None) -> list[dict[str, str]]:
    if merged is None:
        (stage / "FLASHING_NOTE.txt").write_text(
            "No merged Arduino binary was exported by arduino-cli. "
            "Use the manifest and build log to locate board-package flash arguments.\n",
            encoding="utf-8",
        )
        return []

    rel_merged = merged.relative_to(stage).as_posix()
    flash_files = [{"offset": "0x0", "path": rel_merged}]
    sh = stage / "flash.sh"
    sh.write_text(
        "#!/usr/bin/env sh\n"
        "set -eu\n"
        'PORT="${1:-/dev/ttyUSB0}"\n'
        'BAUD="${BAUD:-921600}"\n'
        'SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"\n'
        'cd "$SCRIPT_DIR"\n'
        f'python -m esptool --chip {target} --port "$PORT" --baud "$BAUD" '
        f'write_flash 0x0 "{rel_merged}"\n',
        encoding="utf-8",
    )
    sh.chmod(sh.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    (stage / "flash.bat").write_text(
        "@echo off\r\n"
        "set PORT=%1\r\n"
        "if \"%PORT%\"==\"\" set PORT=COM3\r\n"
        "if \"%BAUD%\"==\"\" set BAUD=921600\r\n"
        f"python -m esptool --chip {target} --port %PORT% --baud %BAUD% "
        f"write_flash 0x0 \"{rel_merged}\"\r\n",
        encoding="utf-8",
    )
    return flash_files


def make_zip(stage: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(stage.rglob("*")):
            if path.is_file():
                archive.write(path, path.relative_to(stage).as_posix())


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--sketch", required=True)
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--core-version", required=True)
    parser.add_argument("--fqbn", required=True)
    parser.add_argument("--target", default="esp32s3")
    args = parser.parse_args()

    artifact_name = f"{safe_name(args.sketch)}-arduino-{safe_name(args.core_version)}"
    stage = args.output_dir / artifact_name
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)

    outputs, merged = copy_outputs(args.build_dir, stage)
    flash_files = write_flash_helpers(stage, args.target, merged)
    manifest = {
        "type": "arduino",
        "sketch": args.sketch,
        "core_version": args.core_version,
        "fqbn": args.fqbn,
        "target": args.target,
        "outputs": outputs,
        "flash_files": flash_files,
    }
    (stage / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    make_zip(stage, args.output_dir / f"{artifact_name}.zip")
    print((args.output_dir / f"{artifact_name}.zip").as_posix())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
