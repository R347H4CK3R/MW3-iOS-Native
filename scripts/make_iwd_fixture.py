#!/usr/bin/env python3
"""Generate tiny, independently authored ZIP/IWD tests. No game assets."""
from pathlib import Path
import argparse
import zipfile

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("directory", type=Path)
    args = ap.parse_args()
    args.directory.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.directory / "valid.iwd", "w", allowZip64=False) as z:
        z.writestr("alpha.txt", b"TEST-IWD-ALPHA\n", compress_type=zipfile.ZIP_DEFLATED)
        z.writestr("tiny.bin", bytes([0, 1, 2, 3]), compress_type=zipfile.ZIP_STORED)
    # Deliberately corrupt a CENTRAL directory checksum, without modifying
    # the compressed bytes, to prove the native reader detects CRC mismatch.
    broken = bytearray((args.directory / "valid.iwd").read_bytes())
    directory_pos = broken.find(bytes.fromhex("504b0102"))
    if directory_pos < 0:
        raise RuntimeError("Synthetic ZIP central directory not found")
    broken[directory_pos + 16] ^= 0x40
    (args.directory / "bad_crc.iwd").write_bytes(broken)
    (args.directory / "not_zip.iwd").write_bytes(b"this is not a ZIP")
    with zipfile.ZipFile(args.directory / "unsafe.iwd", "w", allowZip64=False) as z:
        z.writestr("../outside.txt", b"NOT_ALLOWED", compress_type=zipfile.ZIP_STORED)

if __name__ == "__main__":
    main()
