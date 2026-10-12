# Architecture — MW3 iOS Native

## Product requirements
- Device: iPhone 16 Plus, ARM64 iOS, landscape.
- Modes: Story (single player), Multiplayer, Spec Ops. Mode-specific resources and subsystems must be validated individually; do not label any as playable until tests pass.
- Controls: touch controls routed through the game's own controller interface, plus supported Xbox/PlayStation/MFi controllers. Preserve engine-side aim assist; do not implement gameplay-affecting independent auto-aim.
- Gyro aiming: optional device motion mapped ONLY to camera/aim input. **No tilt steering** and no motion-derived movement/vehicle steering.
- Graphics: a native Metal renderer; user-selectable performance, balanced, quality and near-native internal resolution presets.
- Assets: use user-supplied, lawfully acquired game files via authorized iOS folder access; no forced copy into app documents and no redistribution.

## Engine / WebAssembly feasibility gate
A WebAssembly module is a portable bytecode format, not an ARM64 native executable. To produce a native ARM64 executable, a **valid, legally usable IW5-compatible Wasm module** would have to be ahead-of-time compiled or translated to C and linked with native iOS platform support. A wasm2c-like translation cannot create source from arbitrary MW3 Windows executables; an actual valid Wasm module is required. No verified IW5-compatible WebAssembly input is available in this repository.

Windows IW5 binaries cannot be fed directly to CMake/Xcode/wasm2c. The build must fail explicitly if the engine payload is absent, not generate an IPA falsely labelled as playable.

## Engine interfaces
- app/Source/EngineBridge contains a testable normalized controller bridge and planned mode/preset selection.
- Engine integration requires a documented, authorized IW5 controller input ABI and engine lifecycle API.
- app/Source/MetalRenderer currently contains a precompiled Metal triangle smoke test and a limited D3D9 render-state bridge only. The real IW5 command-stream renderer does not exist. Read-only PE import inspection confirms that original IW5 SP and MP statically import `d3d9.dll` (see `IW5_PE_DEPENDENCIES.md`), so compatibility must be based on Direct3D 9 semantics, not an assumed D3D11 stream.
- app/ShaderTool is reserved for an actual shader pipeline. Tint may translate supported WGSL/SPIR-V to MSL; original IW5 shader formats require a separate proven decoding/translation process.
- iOS GameController/CoreMotion adapters map control signals to the same bridge.
- Build must distinguish host-only test scaffold from actual iOS application.

## Renderer and shaders
1. Obtain authorized engine render-command source or compatible IR and characterize actual IW5 Direct3D 9 usage.
2. Inventory and document commands (resources, render targets, shaders, states, draws, synchronization).
3. Verify extracted shader stage representations and types.
4. Convert only supported intermediate representations through Tint and compile MSL with Apple's compiler.
5. Add GPU frame correctness tests and feature/fallback coverage; do not assume D3D11 equals IW5's actual renderer.

## Game data
Use UIDocumentPicker / security-scoped bookmarks on iOS where supported; file-provider behavior and access persistence depend on provider/entitlements. Preserve original folders and open read-only initially. Use a selectable path inside container only when iOS allows it.

## Modes
- Story: campaign loading, save-state correctness, mission scripting, local AI.
- Multiplayer: map loading, multiplayer simulation, protocol compatibility and server requirements.
- Spec Ops: mission selection, cooperative state and scripted behavior.
No unauthorized online bypass, anti-cheat evasion, or automatic claiming of Plutonium integration.

## Packaging
Hosted GitHub Actions can validate portable C, but native iOS signing/unsigned IPA needs a working Xcode app target, Apple SDK, and actual engine/render components. Provide separate workflows with gates and verifiable build logs. An unsigned IPA still needs proper sideload installation/signing for a specific device.
