# iOS shell (not a game port)

The XcodeGen project.yml defines an installable iOS prototype UI. It demonstrates:
- Mode selection for Story, Multiplayer, Spec Ops
- Graphics preset selection
- Authorized iOS folder picker (no auto-copy of game assets)
- Input-state test HUD and optional gyro aim (no tilt steering)
- A Metal API render-pass smoke test (not a game renderer)

The IW5 engine, original controller ABI, shaders, actual MW3 Metal rendering and playable modes are **not integrated**. Do not label this shell as a playable MW3 IPA. Xcode project generation/build requires macOS with the Apple SDK and XcodeGen.
