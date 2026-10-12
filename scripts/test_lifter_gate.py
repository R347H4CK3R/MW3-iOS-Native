import sys
import tempfile
import unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lifter_gate import assess

class LifterGateTests(unittest.TestCase):
    def test_missing_real_lifter_blocks(self):
        with tempfile.TemporaryDirectory() as tmp:
            sample=Path(tmp)/"sample.exe"
            sample.write_bytes(b"MZ"+bytes(100))
            r=assess(sample,None,None)
            self.assertTrue(r["input_is_pe"])
            self.assertFalse(r["control_flow_graph_present"])
            self.assertFalse(r["mcsema_lifter_present"])
            self.assertFalse(r["mw3_conversion_validated"])

    def test_invalid_data_is_not_a_conversion(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)
            sample=p/"sample.exe"
            sample.write_bytes(b"MZ"+bytes(100))
            cfg=p/"cfg.pb"
            cfg.write_bytes(b"garbage")
            lifter=p/"mcsema-lift"
            lifter.write_bytes(b"not really a lifter")
            r=assess(sample,cfg,lifter)
            self.assertFalse(r["lifted_llvm_bitcode_validated"])
            self.assertFalse(r["native_iOS_executable_validated"])

if __name__=="__main__":
    unittest.main()
