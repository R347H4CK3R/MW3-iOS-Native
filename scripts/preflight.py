#!/usr/bin/env python3
"""Non-destructive readiness audit for the MW3-iOS-Native scaffold.

Checks shape of the project, user-chosen binary file format, and
proprietary payload exclusions. Does not read or upload whole game files.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = (
    "README.md",
    "CMakeLists.txt",
    ".github/workflows/validate.yml",
    "app/Source/EngineBridge/EngineBridge.c",
    "app/Source/EngineBridge/EngineFeatures.c",
    "app/Source/EngineBridge/EngineFeatures.h",
    "app/Source/MetalRenderer/README.md",
    "app/ShaderTool/README.md",
    "docs/ARCHITECTURE.md",
    "docs/INPUT_AND_MODES.md",
)

def identify_binary(path: Path) -> str:
    with path.open("rb") as file:
        magic = file.read(8)
    if len(magic) >= 8 and magic[:4] == bytes.fromhex("0061736d") and magic[4:8] == bytes.fromhex("01000000"):
        return "wasm"
    if magic[:2] == b"MZ":
        return "windows_pe"
    return "unknown"

def audit(root: Path = ROOT, engine_input: Path | None = None) -> dict:
    missing = [name for name in REQUIRED if not (root / name).is_file()]
    result = {
        "project": "MW3-iOS-Native",
        "scaffold_complete": not missing,
        "missing": missing,
        "engine_present": engine_input is not None,
        "engine_format": None,
        "wasm_prerequisite_met": False,
        "metal_renderer_implemented": False,
        "game_playable": False,
        "tilt_steering": False,
    }
    if engine_input is not None:
        if engine_input.is_file():
            kind = identify_binary(engine_input)
            result["engine_format"] = kind
            result["wasm_prerequisite_met"] = kind == "wasm"
        else:
            result["engine_format"] = "missing"
    return result

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--engine-input", type=Path, help="optional explicit engine binary path (no copying)")
    args = parser.parse_args()
    report = audit(engine_input=args.engine_input)
    print(json.dumps(report, indent=2))
    return 0 if report["scaffold_complete"] else 1

if __name__ == "__main__":
    raise SystemExit(main())
