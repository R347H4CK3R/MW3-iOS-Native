"""Tests on an independently generated synthetic x86 PE fixture."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from iw5_abi_audit import inspect, PEError
from test_audit_pe_imports import synthetic_pe

class IW5AbiTests(unittest.TestCase):
    def test_synthetic_imports_and_ordinal(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "toy.exe"
            data = synthetic_pe()
            struct.pack_into("<I", data, 0x80 + 24 + 92, 16)
            path.write_bytes(data)
            result = inspect(path)
            self.assertEqual(result["architecture"], "x86")
            self.assertEqual(result["total_static_imports"], 2)
            self.assertEqual(result["ordinal_imports"], 1)
            self.assertEqual(result["dll_counts"], {"fake9.dll": 2})
            self.assertEqual(result["dll_imports"]["fake9.dll"],
                             ["doThing", "ordinal:1"])
            self.assertFalse(result["native_arm64_ready"])

    def test_corrupt_pe_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "bad.exe"
            path.write_bytes(b"MZ" + bytes(300))
            with self.assertRaises(PEError):
                inspect(path)

    def test_tls_and_relocations_detected(self):
        raw = synthetic_pe()
        opt = 0x80 + 24
        # Mark directories present with synthetic values; detection only.
        struct.pack_into("<I", raw, opt + 92, 16)
        struct.pack_into("<II", raw, opt + 96 + 5*8, 0x1000, 12)
        struct.pack_into("<II", raw, opt + 96 + 9*8, 0x1010, 24)
        struct.pack_into("<II", raw, opt + 96 + 13*8, 0x1020, 24)
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "directory.exe"
            path.write_bytes(raw)
            result = inspect(path)
            self.assertTrue(result["has_relocations"])
            self.assertTrue(result["has_tls_directory"])
            self.assertTrue(result["has_delay_imports"])

if __name__ == "__main__":
    unittest.main()
