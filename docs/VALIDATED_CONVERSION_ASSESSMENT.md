# Validated conversion assessment (2026-10-11)

## Finding
There is **no verified end-to-end conversion of Call of Duty: MW3 (2011) IW5 Windows x86 executable into a playable native iOS ARM64 executable** in the evidence assessed. Do not advertise a working converter or IPA.

## Confirmed documented capabilities
1. **WABT wasm2c** takes WebAssembly binary modules and produces C source/header. It cannot process `iw5sp.exe` or `iw5mp.exe` as input, and these installed executables have PE magic, not a Wasm header.
2. **McSema** supports lifting Windows PE x86 machine code to LLVM bitcode, but its published limitations explicitly include incomplete control-flow recovery, exception modeling, unsupported instructions, and external ABI behavior. Its ability to lift *some* Windows binaries does not validate MW3 or an iOS executable.
3. **rev.ng** demonstrates executable translation for selected Linux binaries; it documents PE support for analysis but most QA is on Linux ELF. This is not documented end-to-end MW3-to-iOS validation.
4. **FEX/Wine/DXMT** can execute some Windows games on ARM64/iOS, but this is runtime translation/emulation requiring JIT and is outside the native AOT conversion requirement.

## Practical validated route — limited to a test
- Obtain a known open-source x86 test application with a stable behavior baseline.
- Lift and recompile it with a pinned implementation of McSema or rev.ng, compare exact expected outputs.
- Compile its LLVM output for the arm64-apple-ios target on a macOS runner and verify its Mach-O CPU type and smoke test in a native iOS host.
- Only after all tests pass, inspect a **permitted sample function** from MW3 for applicability; do not infer whole-game compatibility.

AOT translation is not an automatic solution to Windows system calls, DirectX, x86 calling conventions, thread handling, runtime libraries, engine licensing or content.

## Go / no-go
- **Current game conversion:** NO GO. No MW3 native ARM64 lifted executable validated.
- **Generic tool capability:** Documented, but toolchain integration not yet tested in this repository.
- **Next engineering action:** create and pass a reproducible lifted native ARM64 toy-program test before touching IW5 lifting or claiming an IPA.

## Primary source documentation
- https://github.com/lifting-bits/mcsema
- https://github.com/lifting-bits/mcsema/blob/master/docs/Limitations.md
- https://webassembly.github.io/wabt/doc/wasm2c.1.html
- https://rev.ng/blog/open-sourcing-renvg-decompiler-ui-closed-beta
- https://github.com/jasonkneen/madeira
