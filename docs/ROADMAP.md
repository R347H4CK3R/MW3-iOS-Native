# Engineering roadmap and acceptance gates

## Gate 0 — Repo and requirements
- [x] New, isolated Windows workspace
- [x] Requirements documented (Story / Multiplayer / Spec Ops, touch, engine aim assist, gyro, Metal, no tilt steering)
- [x] Portable controller bridge and normalization unit tests
- [ ] Publish to new private GitHub repository (GitHub connector lacks repository creation)
- [ ] Verify reference renderer project and pin commit (source path names alone do not identify an upstream repository)

## Gate 1 — Engine provenance
- [ ] Determine whether a functional MW3 IW5 WebAssembly build exists and verify rights
- [ ] Establish whether C or Wasm AOT translation is technically feasible
- [ ] Link game entry points without copyrighted binaries in Git
- [ ] Build ARM64 iOS smoke test running engine code

## Gate 2 — Real renderer
- [ ] Document original IW5 rendering API rather than assuming D3D11
- [ ] Implement Metal command decoder in app/Source/MetalRenderer
- [ ] Validate shader extraction, Tint supported input, MSL precompilation
- [ ] Compare screenshots/frame output against authorized reference

## Gate 3 — Input / assets / modes
- [ ] Native touch HUD and GameController adapter
- [ ] CoreMotion gyro-only aiming, calibration and sensitivity UI, no tilt steering
- [ ] Route through native controller entry points to preserve native aim assist
- [ ] Authorized iOS Files folder selection and durable access testing
- [ ] Story, Multiplayer and Spec Ops individual loading tests

## Gate 4 — Performance / packaging
- [ ] Presets up to near-native internal resolution and actual GPU benchmarks
- [ ] Memory/battery/frame-pacing profiling on iPhone 16 Plus
- [ ] Actual iOS Xcode target built on macOS
- [ ] Device install test, logs and crash recovery
- [ ] IPA artifact only once build passes and runtime is verified

## Browser builder
- [ ] Define exactly what the three build files are, sizes, provenance and permitted upload destinations
- [ ] Design GitHub authorization with minimum scopes
- [ ] Implement client-side upload and workflow dispatch with no intermediary storage
- [ ] Test private-repository permissions and server retention claims

Current state is scaffold-only. No functioning MW3 port or playable IPA is claimed.
