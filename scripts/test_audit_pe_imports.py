"""Synthetic fixture tests; never includes proprietary executable bytes."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from audit_pe_imports import audit_pe


def synthetic_pe():
    blob = bytearray(1536)
    blob[:2] = b"MZ"
    struct.pack_into("<I", blob, 0x3C, 0x80)
    blob[0x80:0x84] = bytes.fromhex("50450000")
    struct.pack_into("<H", blob, 0x84, 0x14C)  # i386
    struct.pack_into("<H", blob, 0x86, 1)  # one section
    struct.pack_into("<H", blob, 0x94, 224)  # optional header bytes
    opt = 0x80 + 24
    struct.pack_into("<H", blob, opt, 0x10B)
    # Import data directory at optional + 96 + (index 1 * 8).
    struct.pack_into("<II", blob, opt + 104, 0x1000, 40)
    sec = opt + 224
    struct.pack_into("<IIII", blob, sec + 8, 512, 0x1000, 512, 0x200)
    # PE import descriptor: OriginalFirstThunk, timestamp, chain, name, first.
    struct.pack_into("<IIIII", blob, 0x200, 0x1120, 0, 0, 0x1100, 0x1120)
    blob[0x300:0x30A] = b"fake9.dll" + bytes(1)
    struct.pack_into("<III", blob, 0x320, 0x1140, 0x80000001, 0)
    blob[0x340:0x349] = bytes([0, 0]) + b"doThing"
    blob[0x349] = 0
    return blob


class AuditPETests(unittest.TestCase):
    def test_library_and_symbol_counts(self):
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory) / "toy.exe"
            target.write_bytes(synthetic_pe())
            result = audit_pe(target)
            self.assertEqual(result["architecture"], "x86")
            self.assertEqual(result["imported_libraries"], {"fake9.dll": 2})
            self.assertEqual(result["imported_symbol_count"], 2)

    def test_invalid_magic_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory) / "wrong.exe"
            target.write_bytes(b"not a PE")
            with self.assertRaises(ValueError):
                audit_pe(target)


if __name__ == "__main__":
    unittest.main()
