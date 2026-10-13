# Engineering roadmap and acceptance gates

## Gate 0 — Repo and requirements
- [x] New, isolated Windows workspace
- [x] Requirements documented (Story / Multiplayer / Spec Ops, touch, engine aim assist, gyro, Metal, no tilt steering)
- [x] Portable controller bridge and normalization unit tests
- [x] Published private GitHub repository R347H4CK3R/MW3-iOS-Native; two scaffold validation workflows passed
- [ ] Verify reference renderer project and pin commit (source path names alone do not identify an upstream repository)

## Gate 1 — Engine provenance
- [ ] Determine whether a functional MW3 IW5 WebAssembly build exists and verify rights
- [x] Audit actual IW5 PE32 import names, ordinal imports, TLS and relocation directories (not execution)
- [x] Implement portable host timing primitives and C tests for future native runtime adapters
- [ ] Establish whether a complete IW5 C or Wasm AOT translation is technically and legally feasible
- [ ] Link game entry points without copyrighted binaries in Git
- [ ] Build ARM64 iOS smoke test running engine code

## Gate 2 — Real renderer
- [x] Audit installed PE static imports: IW5 SP and MP both import d3d9.dll (not d3d11.dll)
- [x] Add portable D3D9 render-state prototype and synthetic precompiled Metal triangle probe (NOT IW5)
- [ ] Trace actual D3D9 graphics state and shader formats at runtime before designing full Metal mapping
- [ ] Implement actual IW5 Metal command decoder beyond synthetic D3D9 render-state subset
- [ ] Validate shader extraction, Tint supported input, MSL precompilation
- [ ] Compare screenshots/frame output against authorized reference

## Gate 3 — Input / assets / modes
- [x] Prototype touch HUD and GameController adapter in iOS shell (not connected to IW5)
- [ ] Bind standardized controller state to verified original IW5 input interface
- [ ] CoreMotion gyro-only aiming, calibration and sensitivity UI, no tilt steering
- [ ] Route through native controller entry points to preserve native aim assist
- [x] Prototype iOS Files folder selection and in-place IWD/FastFile header inventory
- [ ] Verify persistent access and real archive entry loading on an iPhone
- [ ] Story, Multiplayer and Spec Ops individual loading tests

## Gate 4 — Performance / packaging
- [x] Prototype four internal-resolution presets in the Metal test view (60/75/90/100%)
- [ ] Connect actual IW5 graphics and benchmark device GPU performance
- [ ] Memory/battery/frame-pacing profiling on iPhone 16 Plus
- [x] XcodeGen spec and native iOS shell source added (non-playable)
- [x] Compile the native iOS shell for Simulator on macOS (GitHub Actions run 38321355186)
- [ ] Build unsigned iPhoneOS shell and inspect its ARM64 payload
- [ ] Install/test shell on a physical iPhone
- [ ] Integrate actual IW5 engine and game renderer
- [ ] Device install test, logs and crash recovery
- [ ] IPA artifact only once build passes and runtime is verified

## Browser builder
- [ ] Define exactly what the three build files are, sizes, provenance and permitted upload destinations
- [ ] Design GitHub authorization with minimum scopes
- [ ] Implement client-side upload and workflow dispatch with no intermediary storage
- [ ] Test private-repository permissions and server retention claims

Current state is scaffold-only. No functioning MW3 port or playable IPA is claimed.
