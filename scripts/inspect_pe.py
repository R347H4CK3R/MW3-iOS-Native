#!/usr/bin/env python3
"""Read-only structural inspection of a PE binary. Never copies game content."""
import argparse
import json
import struct
from pathlib import Path

MACHINES = {0x14c:"x86",0x8664:"x86_64",0xaa64:"ARM64"}
def inspect(path):
    with Path(path).open("rb") as f:
        data=f.read(4096)
        if len(data)<64 or data[:2]!=b"MZ":
            raise ValueError("not a DOS/PE executable")
        off=struct.unpack_from("<I",data,0x3c)[0]
        if off+24>len(data) or data[off:off+4]!=b"PE\0\0":
            raise ValueError("PE header missing in first 4096 bytes")
        machine,sections=struct.unpack_from("<HH",data,off+4)
        sizeopt=struct.unpack_from("<H",data,off+20)[0]
        magic=struct.unpack_from("<H",data,off+24)[0]
        if magic not in (0x10b,0x20b):
            raise ValueError("unsupported optional header")
        entry=struct.unpack_from("<I",data,off+40)[0]
        return {"architecture":MACHINES.get(machine,hex(machine)),
                "format":"PE32" if magic==0x10b else "PE32+",
                "sections":sections,"entry_rva":hex(entry),
                "optional_header_bytes":sizeopt,
                "wasm_module":False,"direct_wasm2c_compatible":False}
if __name__=="__main__":
    parser=argparse.ArgumentParser()
    parser.add_argument("path")
    args=parser.parse_args()
    print(json.dumps(inspect(args.path),indent=2))
