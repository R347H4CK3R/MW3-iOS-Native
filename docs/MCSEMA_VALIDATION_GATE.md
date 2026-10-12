# Established x86 binary lifter validation gate

Selected evaluation candidate: McSema + Remill. This is **not** a claim that McSema converts MW3 to iOS.

## Primary source constraints
- McSema consumes control-flow data produced by mcsema-disass, which in turn requires IDA Pro 7.5+ (the DynInst path is experimental).
- It lifts selected x86 and AMD64 ELF/PE binaries to LLVM bitcode. Lifted bitcode preserves x86 semantics and does not remove Win32/DirectX requirements.
- Published limitations: incomplete control flow, exception recovery, obfuscated/self-modifying code, and external ABI issues.
- rev.ng is a research alternative; its public documentation says PE is supported but most QA is Linux x86-64.

## Reproducible acceptance experiment
1. Pin toolchain revisions, legal license terms and disassembler frontend, then build a clean isolated environment.
2. Use an independently created x86 Windows PE with branching, stack frames, function calls, table lookups and imported Windows APIs.
3. Recover its CFG with a supported frontend and lift to LLVM IR using *actual McSema*. Do not pass off a hand-written instruction translator as McSema.
4. Build an ARM64 Linux or iOS test artifact and compare deterministic outputs with a native x86 baseline; reject any mismatches.
5. Produce an ARM64 iOS Mach-O object using the actual lifted IR and Apple's SDK. Then separately prove native execution on iPhone.
6. Only after those gates inspect a subset of IW5, without committing proprietary code or data.

## Current machine inventory
GamingLaptop: Windows 10, git and Python are available. CMake, clang, gcc, Docker, GitHub CLI and installed WSL distributions are not on PATH. No verified IDA installation is known. GitHub-hosted runners are available.
The iOS ARM64 native compilation proof workflow **passed**, but its source is a simple, independent C file, not lifted PE code.

## Current status
- [x] Tiny synthetic PE x86 -> C -> ARM64 Linux/QEMU passed (custom 2-operation lifter, not general-purpose).
- [x] Separate C -> iOS ARM64 Mach-O compilation passed.
- [ ] McSema/Remill built with a pinned version.
- [ ] Real McSema PE control-flow recovery and lifting passed.
- [ ] Lifted PE IR compiled and executed on ARM64.
- [ ] MW3 IW5 engine translated.
- [ ] Playable iOS runtime.

References:
- https://github.com/lifting-bits/mcsema
- https://github.com/lifting-bits/mcsema/blob/master/docs/Limitations.md
- https://rev.ng/blog/open-sourcing-renvg-decompiler-ui-closed-beta
