# D3D9-to-Metal compatibility — prototype gate

Observed: installed IW5 SP/MP binaries are Windows PE32 x86 and statically import `d3d9.dll`. For original Direct3D 9 state identifiers and definitions, see:
- https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype
- https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dcull
- https://learn.microsoft.com/en-us/windows/win32/direct3d9/culling-state

## Current verified component (portable unit scope)
- Narrow C state tracker with deterministic default values.
- Supports depth enable/write/compare tracking, blend enable and color/alpha blend factors/ops, color write channels, clockwise/counter-clockwise culling, and scissor enable tracking.
- Validates state identifiers and rejects unsupported values; does not pretend it can handle every D3D9 render state.
- C tests cover default state, supported changes, invalid/unsupported commands and no-mutation-on-rejection.

## GPU smoke scope
- Swift Metal probe uses precompiled, independently written `.metal` vertex/fragment shaders to draw a triangle. The four graphics presets now change the Metal test view's internal drawable resolution (60/75/90/100% of screen pixel scale). This does not optimize or render MW3.
- Currently maps color write mask, blend factors/ops and winding/culling. This is **not** a game shader translator.
- D3D9 front faces use clockwise winding by default; culling mapping follows documented `D3DCULL` semantics.
- No depth attachment in this smoke view. Tracked depth state does not imply functional depth testing.
- Dynamic graphics-state changes require pipeline cache and explicit state application before a real draw stream can be translated.

## Blocking gaps before actual IW5 graphics
1. Authorized IW5 engine execution and a traceable drawing interface.
2. Actual command capture/tracing and complete D3D9 semantics, resources, stateblocks, vertex attributes, geometry and frame synchronization.
3. Shader extraction/decoding and conversion from the game's real shader representation to MSL.
4. Validation against known frames from a user-owned installation.
5. Device runtime, power/memory/performance tests.

Unsupported behavior must return a visible error or be separately implemented, never silently interpreted as correct.
