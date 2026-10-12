# C / ARM64 translation prerequisite

WABT wasm2c can emit C from a **WebAssembly binary module**. It cannot convert a Windows PE x86 executable into C, nor can a plain IW5 installation be treated as Wasm.

Potential route:
- Identify a functioning, authorized IW5-compatible Wasm engine module.
- Verify imports and expected platform ABI.
- Convert the module with a pinned WABT release, or use another verified ahead-of-time Wasm compiler.
- Implement missing POSIX/iOS platform services and render/input integrations.
- Compile on an Apple SDK for ARM64, then verify real gameplay.

Currently no compatible MW3 WebAssembly engine module is present or verified.
