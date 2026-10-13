# Native .iwd archive reader — controlled milestone

## User requirement
The game data must be read from its selected folder in iOS Files, not
copied into the app bundle or app-private storage.

## Current implementation
- C ZIP32 parser opens user-authorized path read-only using stdio.
- Reads End of Central Directory and central metadata rather than
  decompressing a whole archive.
- Supports method 0 and raw Deflate method 8 with zlib in memory.
- Rejects unknown methods, encrypted entries, ZIP64, traversal paths.
- Limits output to 1 MiB and compressed input to 16 MiB.
- Validates decompressed length and CRC32.
- Native Swift shell probes the first .iwd, displays directory count,
  and checks one small sample's CRC in memory. Does not save bytes.
- Synthetic C regression tests are run in GitHub Actions using CTest.

## What this does NOT establish
- Parser hasn't been run on iPhone Files-provider MW3 installation yet.
- No complete .ff FastFile decompression or IW5 asset deserialization.
- No actual engine is linked; valid IWD data does not make a playable game.
- No compatibility with ZIP64 or nonstandard ZIP features.
- Full file-provider bookmark persistence not yet device-tested.

## Windows installation caveat
The previously audited 32-bit iw5sp.exe/iw5mp.exe path no longer exists
at its prior location on GamingLaptop. A Steam directory with MW3
archives exists, but at the time of the current inspection it
contains iw5mp_server.exe and lacks those SP/MP client executables.
Keep these installations distinct when discussing IW5 engine provenance.
