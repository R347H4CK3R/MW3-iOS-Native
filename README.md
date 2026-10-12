# MW3-iOS-Native

Private research and engineering project for a native ARM64 iOS adaptation of Call of Duty: Modern Warfare 3 (2011 / IW5).

## Architectural contract

1. **Engine:** Translate relevant x86 engine behavior to C at build time and compile for ARM64 iOS. Translation feasibility, correctness and legal permissions are unresolved; there is no working engine translator in this repository yet.
2. **Graphics:** Implement a Metal renderer at app/Source/MetalRenderer, consuming the compatible translated render-command interface. Shader processing is planned at app/ShaderTool using Tint where supported, with precompilation into the iOS app. IW5 is not assumed to be equivalent to D3D11.
3. **Input:** Touch controls and physical GameController input feed a common controller-state bridge; integration with IW5's original controller code remains to be implemented.
4. **Game files:** Read user-owned game files from a user-selected iPhone folder using permitted security-scoped access. Do not duplicate files into private app storage by default.
5. **Browser builder:** Proposed client-side GitHub authentication and upload of three build inputs into a private repository, followed by a GitHub Actions workflow. The browser builder is **not implemented**; no claim is made about server data retention.
6. **Delivery:** Target unsigned ARM64 iOS IPA, to be built and tested on an Apple-supported build runner.

## Current status

This repository is an **engineering scaffold**, not a port or a playable IPA. It contains a small, host-testable C controller-state bridge and CI validation. It contains no MW3 binaries, copyrighted assets, Metal renderer implementation, IW5 engine source, or functioning iOS Xcode app.

## On this Windows PC

Project root: C:\Users\Chris\Documents\MW3-iOS-Native

To run the bridge unit tests if CMake and a C compiler are available:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

## Important

Never commit game installation files, executable binaries, reverse-engineered proprietary source or licensed shader blobs without verifying the necessary rights. Keep user-owned game data local to the authorized device. See docs/ARCHITECTURE.md, docs/ROADMAP.md, and docs/LEGAL_AND_ASSETS.md.
