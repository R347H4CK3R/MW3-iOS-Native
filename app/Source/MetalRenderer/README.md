# Metal Renderer — Direct3D 9 state compatibility prototype

The installed MW3 SP and MP PE binaries statically import `d3d9.dll` (see `docs/IW5_PE_DEPENDENCIES.md`). This directory now contains a **synthetic rendering smoke test**, not a real game renderer.

## Files
- `D3D9StateBridge.h/.c`: portable C subset of native Direct3D 9 render-state IDs, defaults and validation. Unsupported state commands explicitly return `MW3_D3D9_UNSUPPORTED`.
- `Tests/test_d3d9_state_bridge.c`: host-side C regression tests.
- `ProbeShaders.metal`: synthetic triangle shaders compiled to a Metal library by Xcode on macOS.
- `MetalSmokeRenderer.swift`: draws one triangle with a subset of D3D9 blend, color write and face culling semantics mapped to Metal. Its MTKView internal drawable resolution follows the shell's selected graphics preset (60%, 75%, 90%, or 100% of the screen's native scale).

## Not implemented
- No IW5 engine, render command interception or complete D3D9 state tracking.
- No real D3D9 shader bytecode-to-MSL translation, texture sampling, render targets, vertex buffers, stencil, fixed function, fog, gamma or screen-space coordinate conversions.
- D3D9 depth settings are tracked in the portable C bridge but are NOT used by the triangle probe, which has no depth attachment. No geometry/world rendering.
- This is not a replacement for IW5 and not a playable game. Apple's build runner validates compilation; the triangle has not been visually tested on a physical iPhone.

The test stays isolated to non-proprietary independently authored graphics code.
