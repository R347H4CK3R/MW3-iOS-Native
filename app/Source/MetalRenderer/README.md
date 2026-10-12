# MetalRenderer

Implementation target: native Metal renderer for the appropriate engine render-command representation.

Planned subsystems:
- Resource manager and buffer/texture formats
- Render passes, state, command scheduling and synchronization
- Engine render commands to Metal API mapping
- Offline precompiled shader library and reflection
- Performance instrumentation and graphics presets

There is no Metal renderer implementation or proven IW5 render-command interface in this scaffold. IW5's renderer must be investigated before assuming Direct3D 11 compatibility.
