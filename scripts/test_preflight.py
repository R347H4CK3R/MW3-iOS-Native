import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from preflight import audit, identify_binary, ROOT

class PreflightTests(unittest.TestCase):
    def test_repo_structure(self):
        data = audit(ROOT)
        self.assertTrue(data["scaffold_complete"], data["missing"])
        self.assertFalse(data["game_playable"])
        self.assertFalse(data["tilt_steering"])

    def test_wasm_and_pe_are_distinct(self):
        with tempfile.TemporaryDirectory() as directory:
            candidate = Path(directory) / "engine.wasm"
            candidate.write_bytes(bytes.fromhex("0061736d01000000"))
            self.assertEqual(identify_binary(candidate), "wasm")
            self.assertTrue(audit(ROOT, candidate)["wasm_prerequisite_met"])
            candidate.write_bytes(b"MZ" + bytes(6))
            self.assertEqual(identify_binary(candidate), "windows_pe")
            self.assertFalse(audit(ROOT, candidate)["wasm_prerequisite_met"])

    def test_unknown_format(self):
        with tempfile.TemporaryDirectory() as directory:
            candidate = Path(directory) / "thing.bin"
            candidate.write_bytes(b"HELLO")
            self.assertEqual(identify_binary(candidate), "unknown")

if __name__ == "__main__":
    unittest.main()
