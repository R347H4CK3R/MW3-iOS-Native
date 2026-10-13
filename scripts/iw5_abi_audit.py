#!/usr/bin/env python3
"""Read-only IW5 Windows PE ABI audit.

Emits public Windows API import identifiers and structural counts only.
No x86 code, original game payload, assets, or shader data are exported.
Static import tables are incomplete when libraries are loaded dynamically.
"""
from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path

class PEError(ValueError):
    pass

def u16(data: bytes, off: int) -> int:
    if off < 0 or off + 2 > len(data):
        raise PEError("Truncated PE field")
    return struct.unpack_from("<H", data, off)[0]

def u32(data: bytes, off: int) -> int:
    if off < 0 or off + 4 > len(data):
        raise PEError("Truncated PE field")
    return struct.unpack_from("<I", data, off)[0]

def u64(data: bytes, off: int) -> int:
    if off < 0 or off + 8 > len(data):
        raise PEError("Truncated PE field")
    return struct.unpack_from("<Q", data, off)[0]

CATEGORIES = {
    "d3d9.dll": "graphics", "d3d11.dll": "graphics", "dxgi.dll": "graphics",
    "opengl32.dll": "graphics", "gdi32.dll": "window_graphics",
    "binkw32.dll": "video", "mss32.dll": "audio", "dsound.dll": "audio",
    "winmm.dll": "audio_timing", "steam_api.dll": "steam_service",
    "steamclient.dll": "steam_service",
    "ws2_32.dll": "network", "wininet.dll": "network",
    "kernel32.dll": "windows_runtime", "user32.dll": "windows_runtime",
    "advapi32.dll": "windows_runtime", "ole32.dll": "windows_runtime",
    "shell32.dll": "windows_runtime", "powrprof.dll": "windows_runtime",
}
DYNAMIC_APIS = {
    "loadlibrarya", "loadlibraryw", "loadlibraryexa", "loadlibraryexw",
    "getprocaddress", "freelibrary", "getmodulehandlea", "getmodulehandlew",
}

def inspect(path: Path) -> dict:
    data = path.read_bytes()
    if len(data) < 256 or data[:2] != b"MZ":
        raise PEError("Missing DOS signature")
    pe = u32(data, 0x3c)
    if pe < 64 or pe + 24 > len(data) or data[pe:pe+4] != b"PE" + bytes(2):
        raise PEError("Missing PE signature")
    machine = u16(data, pe+4)
    count = u16(data, pe+6)
    optional_size = u16(data, pe+20)
    optional = pe+24
    magic = u16(data, optional)
    if magic not in (0x10b, 0x20b):
        raise PEError("Unsupported PE optional-header format")
    is64 = magic == 0x20b
    image_base = u64(data, optional+24) if is64 else u32(data, optional+28)
    directories = optional+(112 if is64 else 96)
    n_dirs = u32(data, optional+(108 if is64 else 92))
    if count > 96 or optional+optional_size+count*40 > len(data):
        raise PEError("Invalid PE section table")
    sections = []
    for i in range(count):
        pos = optional + optional_size + 40*i
        sections.append((
            u32(data, pos+12), u32(data, pos+8),
            u32(data, pos+16), u32(data, pos+20)
        ))
    def rva_pos(rva: int) -> int:
        for base, virtual_size, raw_size, raw_start in sections:
            delta = rva - base
            if 0 <= delta < raw_size and delta < max(virtual_size, raw_size):
                out = raw_start + delta
                if out < len(data):
                    return out
        if 0 <= rva < optional+optional_size+count*40 and rva < len(data):
            return rva
        raise PEError("RVA not backed by file data")

    def directory(index: int) -> tuple[int,int]:
        if index >= n_dirs or directories+(index+1)*8 > optional+optional_size:
            return (0,0)
        pos = directories+index*8
        return (u32(data,pos),u32(data,pos+4))

    def cstring(rva: int, max_length: int=512) -> str:
        at = rva_pos(rva)
        end = data.find(bytes(1),at,min(len(data),at+max_length))
        if end < 0:
            raise PEError("Unterminated PE import string")
        return data[at:end].decode("ascii",errors="replace")
    ptr_size = 8 if is64 else 4
    ordinal_bit = (1<<63) if is64 else (1<<31)
    mask = ordinal_bit-1
    imported: dict[str,list[str]] = {}
    ordinal_count = 0
    import_rva, import_size = directory(1)
    if import_rva:
        start = rva_pos(import_rva)
        for i in range(1024):
            p = start+20*i
            if p+20 > len(data):
                raise PEError("Truncated PE import descriptors")
            original = u32(data,p)
            name_rva = u32(data,p+12)
            first = u32(data,p+16)
            if not (original or name_rva or first):
                break
            if not name_rva:
                continue
            dll = cstring(name_rva).lower()
            symbols = imported.setdefault(dll,[])
            thunk_rva = original or first
            if not thunk_rva:
                continue
            start_thunk = rva_pos(thunk_rva)
            for j in range(32768):
                at = start_thunk + ptr_size*j
                value = u64(data,at) if is64 else u32(data,at)
                if value == 0:
                    break
                if value & ordinal_bit:
                    symbols.append("ordinal:"+str(value&0xffff))
                    ordinal_count += 1
                else:
                    try:
                        symbols.append(cstring(int(value&mask)+2))
                    except PEError:
                        symbols.append("unresolved_import_name")
            else:
                raise PEError("Unterminated PE import thunk table")
        else:
            raise PEError("Unterminated PE import descriptor table")

    categories: dict[str,int] = {}
    for dll,names in imported.items():
        category = CATEGORIES.get(dll, "unclassified")
        categories[category] = categories.get(category,0)+len(names)
    allnames = {x.lower() for values in imported.values() for x in values}
    dyn = sorted(DYNAMIC_APIS & allnames)

    # Presence of a directory does not establish functionality; relocation/TLS
    # details also need toolchain support to execute lifted code.
    tls_rva,tls_size = directory(9)
    reloc_rva,reloc_size = directory(5)
    delay_rva,delay_size = directory(13)
    return {
        "file": path.name,
        "architecture": {0x14c:"x86",0x8664:"x86_64",0xaa64:"ARM64"}.get(machine,hex(machine)),
        "format": "PE32+" if is64 else "PE32",
        "section_count":count,
        "entrypoint_rva":hex(u32(data,optional+16)),
        "image_base":hex(image_base),
        "dll_imports":{dll:sorted(names) for dll,names in sorted(imported.items())},
        "dll_counts":{dll:len(names) for dll,names in sorted(imported.items())},
        "import_categories":dict(sorted(categories.items())),
        "total_static_imports":sum(map(len,imported.values())),
        "ordinal_imports":ordinal_count,
        "dynamic_library_apis":dyn,
        "has_relocations":bool(reloc_rva and reloc_size),
        "has_tls_directory":bool(tls_rva and tls_size),
        "has_delay_imports":bool(delay_rva and delay_size),
        "native_arm64_ready":False,
        "limitation":"Static PE imports; does not include dynamically resolved modules, game code or assets",
    }

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binaries",nargs="+",type=Path)
    parser.add_argument("--summary-only",action="store_true",
                        help="Suppress per-symbol names in output")
    args = parser.parse_args()
    results = []
    for path in args.binaries:
        try:
            result=inspect(path)
            if args.summary_only:
                result.pop("dll_imports",None)
            results.append(result)
        except (OSError,PEError,struct.error) as exc:
            results.append({"file":path.name,"error":str(exc)})
    print(json.dumps({"binaries":results},indent=2))
    return 1 if any("error" in item for item in results) else 0

if __name__=="__main__":
    raise SystemExit(main())
