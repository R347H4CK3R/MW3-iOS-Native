# Read-only native C IWD validation — October 12, 2026

## What was tested
GitHub Actions Windows x64 workflow built the repository's independent
IWDReader.c using MinGW-w64 and zlib, tested it with synthetic ZIP
fixtures, and uploaded ONLY the compiled executable plus zlib runtime.

The signed-in PC downloaded that tool to the ignored project
local-fixtures directory, then executed it against the user-owned Steam
MW3 archive directory:

C:\Program Files (x86)\Steam\steamapps\common\Call of Duty Modern Warfare 3\main

The archives were opened READ ONLY. **No original game file or decoded
entry content was copied, committed to GitHub, or uploaded.**

## Verified results on GamingLaptop
- Windows native executable first validated main/iw_00.iwd.
- 2,527 entries indexed, 2,527 method-8 Deflate entries.
- A 77-byte sample decoded into memory and its CRC32 matched.
- Additional scan of all IWD archives found in this CURRENT Steam
  directory: **24/24** ZIP indexes succeeded.
- 22 archives had eligible small entries decoded and CRC32-checked.
- 2 archives had no suitable small sample; index succeeded, but CRC of
  an entry in those 2 was NOT verified.
- No sampled archive validation failures.

These counts are for the currently available Steam installation, not
the earlier separately audited installation under Program Files (x86)
which had 47 IWD archives at the time. The earlier directory is no
longer present. The current directory includes iw5mp_server.exe and
does not currently contain iw5sp.exe or iw5mp.exe.

## Engine gate
This establishes an independent compatible ZIP reader and in-memory
sample decode on Windows. iOS ARM64 builds the same C reader, but it has
not yet been installed and tested with the actual MW3 folder via Files
on an iPhone. No .ff decompression or IW5 executable is integrated;
this is not a playable game.

Workflow: .github/workflows/windows-iwd-probe.yml
Local runner: scripts/verify_owned_iwd_windows.ps1
