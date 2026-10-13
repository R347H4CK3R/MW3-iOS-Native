# Portable host timing — initial ABI preparation

The installed IW5 Windows executables statically import GetTickCount,
QueryPerformanceCounter, QueryPerformanceFrequency and Sleep.

This independently authored C implementation provides equivalent
host-side monotonic timer, millisecond tick and sleep primitives on
POSIX/Linux/iOS. Unit tests check monotonicity and timing behavior.

It is NOT the IW5 runtime, not an implementation of every Win32 API,
and does NOT intercept or satisfy the Windows PE executable's import
table automatically. Lifted engine code would still need calling-
convention bridging, module/linker integration, more Win32 API shims,
native entry points, graphics, file system, service and audio support.
