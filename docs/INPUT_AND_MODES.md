# Controls and modes — specification

## Modes
Story (single-player campaign), Multiplayer and Spec Ops are separate launch targets within a single app. Each must independently satisfy boot, asset loading, control, rendering and gameplay verification.

## Input
Touch UI acts as a game controller: left virtual stick for movement, right virtual stick or drag for looking, virtual triggers and face buttons. Physical Xbox, PlayStation and MFi controllers use Apple's GameController.

Actual engine controller entry points must be identified and used, rather than simulating keyboard and mouse input or implementing distinct combat rules. Native aim assist must remain engine-controlled, where available and legally implemented.

Optional gyroscopic **aim adjustment only** via CoreMotion. User can calibrate, tune sensitivity, and disable it. **No tilt steering**: orientation/motion must never change steering, movement or vehicle throttle.

## Graphics presets
- Performance: conservative render scale / effects
- Balanced: moderate quality and render scale
- Quality: higher render scale and enabled effects
- Near Native: near physical display resolution *where device performance permits*

Profiles are desired behavior, not currently benchmarked performance. Renderer does not exist yet.

## Acceptance tests
- Touch and physical controller signals yield equivalent standardized controller state.
- Engine's original aim assist receives standardized controller data; no separate auto-aim layer.
- Gyro disabled means no motion-derived aim; gyro enabled means movement/steering unchanged.
- Story, Multiplayer, Spec Ops each boot and load user-selected assets.
