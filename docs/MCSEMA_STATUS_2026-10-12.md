# McSema reproducibility checkpoint — October 12, 2026

The original lifting-bits/mcsema repository is archived (since August 23, 2022). Its documented supported CFG recovery path uses a licensed IDA Pro 7.5+ installation; DynInst is experimental and unsupported. This is not a turnkey Windows x86 PE -> native iOS compiler.

Current GamingLaptop: no installed Docker, clang, CMake, or configured Linux/WSL distribution was detected; no IDA license/installation verified. Existing GitHub Actions runners can compile C for ARM64, but cannot manufacture McSema CFG files.

The repository's `scripts/lifter_gate.py` rejects lifting claims when the CFG or real lifter executable is absent. This is a prerequisite check only and is not proof that an arbitrary CFG or executable is valid. Tests ensure the presence check never reports a valid MW3 conversion.

Source: https://github.com/lifting-bits/mcsema

Next genuine validation must obtain a supported binary analysis frontend, pin and build McSema/Remill, recover CFG on a rights-cleared x86 Windows program, perform an actual lift, and validate execution. A working native iOS MW3 build has not been demonstrated.
