# MW3 engine source feasibility — 2026-10-12

- Installed iw5sp.exe and iw5mp.exe are Windows x86 PE32, not a WebAssembly module.
- rev.ng toy proof translates a synthetic x86 PE function to C and compiles for iOS ARM64; it does not translate IW5.
- Public k8se10/MW32011NCP provides a Windows DLL-based patch and original-engine hooks, not an iOS replacement engine. It documents a September 2026 x64 executable rebuild. Our installed PC executables remain x86; layouts, offsets and calling conventions must not be assumed compatible.
- cydiakk/open-iw5 calls itself experimental and unfinished and requires MW3 dedicated server files.
- AlterWare's archived MW3 client depends on original game binaries and is unsupported in 2026.

## Current go/no-go
- Functioning rights-cleared IW5 WebAssembly engine: NOT FOUND.
- Licensed complete IW5 implementation compatible with this installation: NOT FOUND.
- Full AOT translation of IW5 into playable iOS ARM64: NOT DEMONSTRATED.
- Actual D3D9-to-Metal IW5 command pipeline and IW5 shader IR conversion: NOT IMPLEMENTED.

No playable MW3 IPA can be claimed from the current project. A compiled test shell is not a game engine.

## Next credible engineering gates
1. Authoritative source provenance and legal permission for a compatible engine.
2. Verify real engine input, dependency ABI and platform hooks rather than assuming functions from x64 Windows mods can be transplanted.
3. Read and verify asset containers in place, without uploading copyrighted payloads to GitHub.
4. Only claim gameplay after original IW5 functions execute, a game map loads, and a frame is rendered on target device.

External research: https://github.com/k8se10/MW32011NCP ; https://github.com/cydiakk/open-iw5 ; https://github.com/alterware/client-files
