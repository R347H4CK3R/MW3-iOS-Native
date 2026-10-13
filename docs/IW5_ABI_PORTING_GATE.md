# Actual IW5 ABI compatibility gate — 2026-10-12

This audit reads only PE headers, static imported Windows API names,
and directory metadata from the MW3 2011 Windows executables on GamingLaptop.
No game code, original executable, assets, shaders or user data were copied
into GitHub.

## Audited executables

| Metadata | iw5sp.exe (Story) | iw5mp.exe (Multiplayer / Spec Ops context) |
| --- | ---: | ---: |
| CPU/format | x86 PE32 | x86 PE32 |
| Static imported entries | 378 | 377 |
| Imported by ordinal | 27 | 29 |
| Sections | 6 | 6 |
| PE TLS directory | Present | Present |
| PE relocation directory | Not present | Not present |
| PE delay-import directory | Not present | Not present |
| Direct3D library | d3d9.dll | d3d9.dll |

Both executables statically reference GetProcAddress, LoadLibraryA,
GetModuleHandleA/W and FreeLibrary. Therefore static import coverage
**does not establish** runtime dependency completeness.

Not having a relocation directory can complicate any attempt to
execute a translated image with original absolute pointers. Presence of
a TLS directory means translated startup needs to reproduce the
relevant threading and initialization behavior.

## Windows API coverage categories (static import entries)

| API family | Story | Multiplayer |
| --- | ---: | ---: |
| Windows runtime | 247 | 225 |
| Networking | 27 | 27 |
| Steam services | 16 | 20 |
| Bink video | 21 | 21 |
| MSS / DSound audio | 50 | 58 |
| WinMM audio/timing | 3 | 12 |
| GDI graphics/windows | 13 | 13 |
| Direct3D9 | 1 | 1 |

The Direct3D 9 function imported statically is Direct3DCreate9.
Calling into its COM interfaces can dispatch far more methods than one
import entry suggests.

## First port-native platform building block

app/Source/HostRuntime/HostClock.c independently implements a monotonic
nanosecond counter (frequency 1e9), a wrapping millisecond tick value,
and sleep on POSIX/iOS. This matches timing concepts used by imported
GetTickCount, QueryPerformanceCounter/Frequency and Sleep.

**This is not an automatic Win32 loader, ABI wrapper or execution
bridge.** The game imports still point at Win32 functions; a lifted
engine would require explicit calling-convention mapping, Windows
memory/exception handling, thread-local storage, a relocated memory
image or equivalent data translation, graphics, audio, networking and
legitimate service integration.

## Reproduce locally

    python scripts/iw5_abi_audit.py --summary-only "C:\Program Files (x86)\Call of Duty Modern Warfare 3\iw5sp.exe" "C:\Program Files (x86)\Call of Duty Modern Warfare 3\iw5mp.exe"

The script emits import names only when --summary-only is omitted.
Runtime-loaded modules, undocumented self-modification, x86 ABI
assumptions, Steam services and IW5 assets are not proven compatible.
