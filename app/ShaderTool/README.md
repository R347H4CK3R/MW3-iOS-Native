# ShaderTool

Planned offline shader pipeline, for verified rights-cleared shader inputs:
1. Inspect and identify actual IW5 shader storage and bytecode formats.
2. Convert supported original shader representation into WGSL or SPIR-V, preserving semantics.
3. Use a pinned, license-compliant Tint tool to produce MSL.
4. Use Apple's Metal compiler to precompile and package the metallib.
5. Validate with render tests.

Tint is not a direct parser of arbitrary IW5 shader data, and no shader translator currently exists here. No proprietary source or prebuilt Tint binary is included.
