#!/usr/bin/env python3
"""Gate for genuine McSema input: reject PE or fake CFG instead of claiming a lift."""
import argparse
import json
from pathlib import Path

def assess(binary: Path, cfg: Path | None, lifter: Path | None):
    with binary.open("rb") as f:
        magic=f.read(4)
    checks={
        "input_exists": True,
        "input_is_pe": magic[:2]==b"MZ",
        "control_flow_graph_present": bool(cfg and cfg.is_file() and cfg.stat().st_size>0),
        "mcsema_lifter_present": bool(lifter and lifter.is_file()),
        "lifted_llvm_bitcode_validated": False,
        "native_iOS_executable_validated": False,
        "mw3_conversion_validated": False,
    }
    return checks

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("binary",type=Path)
    ap.add_argument("--cfg",type=Path)
    ap.add_argument("--lifter",type=Path)
    args=ap.parse_args()
    if not args.binary.is_file():
        ap.error("Input binary missing")
    report=assess(args.binary,args.cfg,args.lifter)
    print(json.dumps(report,indent=2))
    # The gate MUST NOT claim a usable lifter when either prerequisite is missing.
    return 0 if report["control_flow_graph_present"] and report["mcsema_lifter_present"] else 2

if __name__=="__main__":
    raise SystemExit(main())
