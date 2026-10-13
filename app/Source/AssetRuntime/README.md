# Read-only IWD asset loading prototype

This independent C implementation scans ZIP central-directory structures in
user-owned MW3 `.iwd` archives without copying the archives into app storage.
It can decode a **bounded, named** ZIP entry using zlib (method 0/store or
method 8/raw Deflate), verify its CRC32, and return bytes in caller-owned memory.
No extracted data is written to disk.

The iOS shell reports the first indexed IWD's entry count and, if a small
entry is available, the result of decoding one sample in memory.

## Bounds / limitations
- ZIP32 only, one disk, no encryption, no ZIP64, no signature confusion
- Basic printable ASCII paths only; traversal, backslash and colon rejected
- Max decompressed sample is 1 MiB, max compressed entry is 16 MiB
- No complete ZIP entry streaming, metadata caching or all-archive indexing
- No game map, D3D9 shader or IW5 FastFile parsing; no engine execution
- Sample is read in memory and discarded after verification

## Test
Linux GitHub Actions CTest generates a tiny archive containing an
independently authored Deflate file, stored binary and intentionally
invalid archives. Tests exercise both methods, CRC verification,
unsupported/traversal rejection and missing entries.

Does not automatically accept arbitrary ZIP features present in some
third-party mod archives. Real iPhone Files-provider access must be
tested on device.
