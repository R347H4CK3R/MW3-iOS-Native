# Build inputs contract — unresolved

The described web builder is expected to upload exactly **three build files** to a private GitHub repository, but their formats, ownership, sizes, and contents are not defined. No such files have been invented or uploaded.

Proposed separation:
- Openly shareable project source and scripts remain in Git.
- Engine input is an authentic, legally usable IW5-compatible Wasm module, if one can be identified.
- Shader input must be a verified shader representation that Tint or an approved conversion step can translate.
- User-owned MW3 game data stays on the phone and is read through explicit folder permission.

No real engine or shader input has been validated; CI must not label a scaffold build as a playable MW3 IPA.
