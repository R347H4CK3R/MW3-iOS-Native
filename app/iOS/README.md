# iOS shell (not a game port)

The XcodeGen project.yml defines an installable iOS prototype UI. It demonstrates:
- Mode selection for Story, Multiplayer, Spec Ops
- Graphics preset selection that changes internal render resolution for the Metal probe only, not MW3 gameplay
- Authorized iOS folder picker (no auto-copy of game assets), plus read-only IWD/FastFile signature counts from the selected directory
- Input-state test HUD and optional gyro aim (no tilt steering)
- A Metal API render-pass smoke test (not a game renderer)

The IW5 engine, original controller ABI, shaders, actual MW3 Metal rendering and playable modes are **not integrated**.

A separate, manually triggered GitHub workflow (`ios-device-package.yml`) compiles the iPhoneOS shell as an unsigned ARM64 test IPA. It is clearly named `MW3NativeShell-UNSIGNED-NOT-A-GAME.ipa`, is not signed for installation, and is NOT a playable MW3 game. Do not label this shell as a playable MW3 IPA. Xcode project generation/build requires macOS with the Apple SDK and XcodeGen.
