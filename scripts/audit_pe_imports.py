#!/usr/bin/env python3
"""Read-only Windows PE DLL import inventory.

Only emits library names and imported-entry counts (no code or file payload).
Static imports may miss dependencies loaded dynamically at runtime.
"""
from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path


def _u16(data: bytes, at: int) -> int:
    if at < 0 or at + 2 > len(data):
        raise ValueError("Truncated PE header")
    return struct.unpack_from("<H", data, at)[0]


def _u32(data: bytes, at: int) -> int:
    if at < 0 or at + 4 > len(data):
        raise ValueError("Truncated PE header")
    return struct.unpack_from("<I", data, at)[0]


def audit_pe(path: Path) -> dict:
    data = path.read_bytes()
    if len(data) < 128 or data[:2] != b"MZ":
        raise ValueError("Not a Windows PE executable")
    pe = _u32(data, 0x3C)
    if pe + 24 > len(data) or data[pe:pe + 4] != bytes.fromhex("50450000"):
        raise ValueError("PE signature not found")

    machine = _u16(data, pe + 4)
    section_count = _u16(data, pe + 6)
    opt_size = _u16(data, pe + 20)
    opt = pe + 24
    magic = _u16(data, opt)
    if magic not in (0x10B, 0x20B):
        raise ValueError("Unknown PE optional-header format")
    pe32plus = magic == 0x20B
    directory_start = opt + (112 if pe32plus else 96)
    if opt_size < (120 if pe32plus else 104):
        raise ValueError("PE optional header lacks an import directory")
    import_rva = _u32(data, directory_start + 8)
    import_size = _u32(data, directory_start + 12)
    sec_off = opt + opt_size

    sections = []
    if section_count > 96:
        raise ValueError("Unexpected number of PE sections")
    for i in range(section_count):
        at = sec_off + i * 40
        sections.append({
            "virtual_size": _u32(data, at + 8),
            "virtual_address": _u32(data, at + 12),
            "raw_size": _u32(data, at + 16),
            "raw_address": _u32(data, at + 20),
        })

    def rva_to_offset(rva: int) -> int:
        for sec in sections:
            delta = rva - sec["virtual_address"]
            if 0 <= delta < min(max(sec["virtual_size"], sec["raw_size"]), sec["raw_size"]):
                pos = sec["raw_address"] + delta
                if pos < len(data):
                    return pos
        if 0 <= rva < min(sec_off + section_count * 40, len(data)):
            return rva
        raise ValueError(f"RVA not backed by on-disk bytes: 0x{rva:x}")

    def read_cstring(rva: int) -> str:
        pos = rva_to_offset(rva)
        end = data.find(bytes(1), pos, min(pos + 512, len(data)))
        if end == -1:
            raise ValueError("Unterminated import name")
        return data[pos:end].decode("ascii", errors="replace")

    imports = {}
    if import_rva:
        start = rva_to_offset(import_rva)
        bound = min(len(data), start + (import_size if import_size >= 20 else 20 * 512))
        width = 8 if pe32plus else 4
        ordinal_bit = 1 << (width * 8 - 1)
        for i in range(512):
            pos = start + i * 20
            if pos + 20 > bound:
                break
            original = _u32(data, pos)
            name_rva = _u32(data, pos + 12)
            first = _u32(data, pos + 16)
            if not (original or name_rva or first):
                break
            if not name_rva:
                continue
            name = read_cstring(name_rva).lower()
            if not name:
                continue
            thunk = original or first
            count = 0
            if thunk:
                thunk_start = rva_to_offset(thunk)
                for j in range(16384):
                    where = thunk_start + j * width
                    if where + width > len(data):
                        break
                    raw = struct.unpack_from("<Q" if pe32plus else "<I", data, where)[0]
                    if raw == 0:
                        break
                    count += 1
            imports[name] = imports.get(name, 0) + count

    return {
        "architecture": {0x14C: "x86", 0x8664: "x86_64", 0xAA64: "ARM64"}.get(machine, hex(machine)),
        "format": "PE32+" if pe32plus else "PE32",
        "imported_libraries": dict(sorted(imports.items())),
        "library_count": len(imports),
        "imported_symbol_count": sum(imports.values()),
        "limitations": "Static PE imports only; dynamic LoadLibrary/GetProcAddress dependencies omitted",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="+", type=Path)
    args = parser.parse_args()
    reports = []
    for path in args.paths:
        try:
            reports.append({"file": path.name, **audit_pe(path)})
        except (OSError, ValueError, struct.error) as exc:
            reports.append({"file": path.name, "error": str(exc)})
    print(json.dumps(reports, indent=2))
    return 1 if any("error" in r for r in reports) else 0


if __name__ == "__main__":
    raise SystemExit(main())
