import sys
import tempfile
import unittest
import zipfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scan_iw5_layout import scan


class IW5LayoutTests(unittest.TestCase):
    def test_signed_and_unsigned_fastfiles_and_zip(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "main").mkdir()
            (root / "zone" / "english").mkdir(parents=True)
            with zipfile.ZipFile(root / "main" / "test.iwd", "w") as archive:
                archive.writestr("textures/test.txt", "synthetic only")
            (root / "zone" / "english" / "story.ff").write_bytes(b"IWffu100" + bytes(12))
            (root / "zone" / "english" / "mp.ff").write_bytes(b"IWff0100" + bytes(12))
            result = scan(root)
            self.assertEqual(result["iwd_count"], 1)
            self.assertEqual(result["iwd_zip_opened"], 1)
            self.assertEqual(result["iwd_entries"], 1)
            self.assertEqual(result["fastfile_count"], 2)
            self.assertEqual(result["fastfile_signed"], 1)
            self.assertEqual(result["fastfile_unsigned"], 1)
            self.assertTrue(result["container_headers_ready"])
            self.assertFalse(result["engine_ready"])

    def test_bad_container_cannot_pass(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "main").mkdir()
            (root / "zone" / "english").mkdir(parents=True)
            (root / "main" / "broken.iwd").write_bytes(b"bad")
            (root / "zone" / "english" / "broken.ff").write_bytes(b"unknown!")
            result = scan(root)
            self.assertFalse(result["container_headers_ready"])
            self.assertEqual(result["iwd_zip_opened"], 0)
            self.assertEqual(result["fastfile_unrecognized"], 1)
            self.assertTrue(result["errors"])

    def test_missing_folders(self):
        with tempfile.TemporaryDirectory() as directory:
            result = scan(Path(directory))
            self.assertFalse(result["container_headers_ready"])
            self.assertFalse(result["main_present"])
            self.assertFalse(result["zone_english_present"])

if __name__ == "__main__":
    unittest.main()
