# IW5 x86 to native ARM64 feasibility — 2026-10-11

## Observed starting point
- The original installed `iw5sp.exe` is Windows PE, not WebAssembly (verified by local header preflight).
- No legally usable IW5 WebAssembly source or original game source has been identified.
- Native iOS target requires arm64 Mach-O, not Windows PE or ARM64 Windows.
- This repo has only an experimental non-playable native iOS shell.

## Candidate comparison

| Path | What it does | IW5-to-iOS assessment |
| --- | --- | --- |
| WABT wasm2c | WebAssembly module -> C and header | Not applicable until an authentic compatible Wasm engine exists. Cannot parse PE files. |
| McSema / Remill | Recover control flow and lift x86 PE instructions to LLVM IR | Best candidate for feasibility experiments on extracted, authorized *non-game* test functions; not a turnkey whole-game converter. |
| rev.ng / other lifters | Binary analysis and LLVM-based IR translation | Research alternative. Recovering correct dynamic behavior and Win32 ABI remains a blocker. |
| FEX | Runtime x86/x86-64 emulation on ARM64 Linux (often Wine for Windows apps) | Demonstrates x86->ARM64 dynamic translation, but is not a native iOS app port, requires substantial operating-system support, and may depend on JIT constraints. |
| Full reimplementation | Recreate clean-room engine with original assets loaded locally | Does not preserve original engine binary; extremely large effort and compatibility work. |

## Chosen next engineering route
1. **Inventory PE without changing it**: architecture, sections, imports, dependencies, relocations, entrypoint, address ranges, packed/protected sections and DLL interfaces. Record metadata, never commit raw proprietary content.
2. **Build representative independent tests**: lift a small permitted x86 Windows test application using a reproducibly pinned lifter; compile to ARM64 Mach-O on macOS and compare outputs against x86 baseline.
3. **Proof-of-concept PE lifting**: only after validation and rights review, test recovered basic blocks/functions from legitimately accessible IW5 installation and log unsupported constructs.
4. **Platform bridge**: map the Windows user-mode API surface to iOS equivalents; implement 32-bit pointer/address-space and calling convention emulation as necessary, and replace incompatible APIs.
5. **Render/input/data**: bridge original engine lifecycle/controller commands to native iOS input, decode verified shader formats, build genuine Metal renderer, and access game files with iOS folder permissions.
6. **Device acceptance**: pass functional Story, Multiplayer, and Spec Ops tests, no tilt steering; profile performance and memory.

## Fundamental constraints
LLVM bitcode is not source-level IW5 C and lifted code retains source-CPU semantics (register flags, 32-bit address model and runtime behavior). Targeting ARM64 does not magically change Win32 ABI into Darwin APIs. Dynamic code, callbacks, thread-local storage, self-modifying code, plugin interfaces, exceptions, software protections and 32-bit address assumptions may each block a successful port.

## Criteria to call it successful
- Actual ARM64 Mach-O running on an iPhone, no x86 CPU emulator at runtime
- Correct Win32 API replacement for exercised code paths
- Reproducible engine startup and original content loading
- Working real Metal gameplay rendering
- Correct controller/aim-assist behavior, story/MP/Spec Ops functional tests

## Sources
- McSema: https://github.com/lifting-bits/mcsema
- McSema limitations: https://github.com/lifting-bits/mcsema/blob/master/docs/Limitations.md
- WABT wasm2c: https://webassembly.github.io/wabt/doc/wasm2c.1.html
- FEX: https://github.com/FEX-Emu/FEX

**Current result: ARM64 conversion has NOT been demonstrated for MW3. This is a feasibility design, not a game port.**
