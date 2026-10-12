#!/usr/bin/env python3
"""Read-only IW5 file-layout audit, with no asset extraction or uploads.

This reports only counts, container signatures and malformed-file warnings.
It does not assume that finding containers means the engine can load them.
"""
from __future__ import annotations

import argparse
import json
import zipfile
from pathlib import Path

FASTFILE_SIGNATURES = {b"IWffu100": "unsigned", b"IWff0100": "signed"}

def scan(root: Path) -> dict:
    root = root.expanduser().resolve()
    main = root / "main"
    zone = root / "zone" / "english"
    out = {
        "layout": "IW5 Windows installation",
        "main_present": main.is_dir(),
        "zone_english_present": zone.is_dir(),
        "iwd_count": 0,
        "iwd_zip_opened": 0,
        "iwd_entries": 0,
        "fastfile_count": 0,
        "fastfile_signed": 0,
        "fastfile_unsigned": 0,
        "fastfile_unrecognized": 0,
        "errors": [],
        "engine_ready": False,
    }
    if main.is_dir():
        for path in sorted(main.glob("*.iwd")):
            if not path.is_file() or path.is_symlink():
                continue
            out["iwd_count"] += 1
            try:
                with zipfile.ZipFile(path) as archive:
                    out["iwd_entries"] += len(archive.infolist())
                    out["iwd_zip_opened"] += 1
            except (OSError, zipfile.BadZipFile, zipfile.LargeZipFile) as exc:
                out["errors"].append(f"{path.name}: {exc.__class__.__name__}")
    if zone.is_dir():
        for path in sorted(zone.glob("*.ff")):
            if not path.is_file() or path.is_symlink():
                continue
            out["fastfile_count"] += 1
            try:
                with path.open("rb") as handle:
                    header = handle.read(12)
                variant = FASTFILE_SIGNATURES.get(header[:8])
                if variant:
                    out[f"fastfile_{variant}"] += 1
                else:
                    out["fastfile_unrecognized"] += 1
                    out["errors"].append(f"{path.name}: unrecognized FastFile signature")
                if len(header) < 12:
                    out["errors"].append(f"{path.name}: truncated FastFile header")
            except OSError as exc:
                out["fastfile_unrecognized"] += 1
                out["errors"].append(f"{path.name}: {exc.__class__.__name__}")
    out["container_headers_ready"] = (
        out["main_present"] and out["zone_english_present"]
        and out["iwd_count"] > 0 and out["fastfile_count"] > 0
        and out["iwd_zip_opened"] == out["iwd_count"]
        and out["fastfile_unrecognized"] == 0 and not out["errors"]
    )
    return out

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("folder", type=Path)
    args = parser.parse_args()
    result = scan(args.folder)
    print(json.dumps(result, indent=2))
    return 0 if result["container_headers_ready"] else 1

if __name__ == "__main__":
    raise SystemExit(main())
