# MW3-iOS-Native

Private research and engineering project for a native ARM64 iOS adaptation of Call of Duty: Modern Warfare 3 (2011 / IW5).

## Architectural contract

1. **Engine:** Translate relevant x86 engine behavior to C at build time and compile for ARM64 iOS. Translation feasibility, correctness and legal permissions are unresolved; there is no working engine translator in this repository yet.
2. **Graphics:** Implement a Metal renderer at app/Source/MetalRenderer, consuming the compatible translated render-command interface. Shader processing is planned at app/ShaderTool using Tint where supported, with precompilation into the iOS app. The installed IW5 executables statically import **Direct3D 9** (`d3d9.dll`), not Direct3D 11. Any Metal translation must start from actual IW5/D3D9 behavior (see `docs/IW5_PE_DEPENDENCIES.md`).
3. **Input:** Touch controls and physical GameController input feed a common controller-state bridge; integration with IW5's original controller code remains to be implemented. A read-only audit of the installed IW5 PE import tables and an independent POSIX/iOS host-clock module are included (see docs/IW5_ABI_PORTING_GATE.md).
4. **Game files:** Read user-owned game files from a user-selected iPhone folder using permitted security-scoped access. Do not duplicate files into private app storage by default.
5. **Browser builder:** Proposed client-side GitHub authentication and upload of three build inputs into a private repository, followed by a GitHub Actions workflow. The browser builder is **not implemented**; no claim is made about server data retention.
6. **Delivery:** Target unsigned ARM64 iOS IPA, to be built and tested on an Apple-supported build runner.

## Current status

This repository is an **engineering scaffold**, not a port or a playable IPA. It includes a portable C controller-state bridge with tests and an experimental SwiftUI iOS shell with a precompiled Metal triangle probe with a limited D3D9 state bridge, touch/gamepad input, gyro-only aim input, folder selection, mode and graphics preset selectors. These are NOT integrated with IW5. It contains no MW3 binaries, copyrighted assets, game shader translation, real IW5 renderer or native game engine. The iOS shell is built via XcodeGen on macOS; its compilation is not yet verified.

## On this Windows PC

Project root: C:\Users\Chris\Documents\MW3-iOS-Native

To run the bridge unit tests if CMake and a C compiler are available:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

## Important

Never commit game installation files, executable binaries, reverse-engineered proprietary source or licensed shader blobs without verifying the necessary rights. Keep user-owned game data local to the authorized device. See docs/ARCHITECTURE.md, docs/ROADMAP.md, and docs/LEGAL_AND_ASSETS.md.
