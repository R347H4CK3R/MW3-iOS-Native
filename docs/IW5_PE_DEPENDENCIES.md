# IW5 Windows executable import audit

Read-only inspection of the user's installed Call of Duty: Modern Warfare 3 (2011) executables on GamingLaptop. No original game binaries or assets were copied or uploaded.

## Observed PE headers
- `iw5sp.exe`: PE32 x86, 6 sections
- `iw5mp.exe`: PE32 x86, 6 sections
- **Neither is a WebAssembly module or a native iOS executable.**

## Static DLL import counts

| DLL | Single-player | Multiplayer |
| --- | ---: | ---: |
| advapi32.dll | 15 | 11 |
| binkw32.dll | 21 | 21 |
| d3d9.dll | 1 | 1 |
| dsound.dll | 0 | 2 |
| gdi32.dll | 13 | 13 |
| kernel32.dll | 171 | 150 |
| mss32.dll | 50 | 56 |
| ole32.dll | 1 | 0 |
| powrprof.dll | 1 | 1 |
| shell32.dll | 1 | 1 |
| steam_api.dll | 16 | 20 |
| user32.dll | 58 | 62 |
| winmm.dll | 3 | 12 |
| ws2_32.dll | 27 | 27 |
| **Total imports** | **378** | **377** |

**Important renderer consequence:** both executables statically import `d3d9.dll`, not `d3d11.dll`. The MetalRenderer design cannot assume an original Direct3D 11 command stream. Direct3D 9 resource/state/shader behavior, alongside any engine-specific abstraction, must be reverse-mapped or reimplemented with authorization before game frames are possible.

## Native iOS compatibility categories

- Direct3D 9 / Windows presentation: Metal pipeline, texture/sampler/state and shader translation, window and device-loss semantics.
- Win32 Kernel32 / User32 / GDI32 / Advapi32 / Shell32 / Powrprof: process/runtime, threading, timing, filesystem and OS interfaces.
- MSS32 / Bink / DSound / WinMM: audio mixing/decoding and cutscenes with verified codecs/rights.
- WS2_32 / Steam API: networking/session interfaces, account/service requirements, legitimate server compatibility.

Counts are from **static PE import descriptors only**; DLLs/symbols dynamically loaded with `LoadLibrary` and `GetProcAddress` will not appear. One D3D9 import does not imply only one Direct3D call during gameplay.

To regenerate the local report without copying binaries:

```powershell
python scripts/audit_pe_imports.py "C:\\Program Files (x86)\\Call of Duty Modern Warfare 3\\iw5sp.exe" "C:\\Program Files (x86)\\Call of Duty Modern Warfare 3\\iw5mp.exe"
```

## Current gate
Native ARM64 translation of a synthetic PE function has been demonstrated. **No working IW5 engine translation or D3D9-to-Metal renderer has been demonstrated**.
