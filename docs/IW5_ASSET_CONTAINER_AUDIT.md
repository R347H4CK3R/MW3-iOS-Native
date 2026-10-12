# Read-only IW5 asset-container audit

Source: user-owned MW3 (2011) installation at C:\Program Files (x86)\Call of Duty Modern Warfare 3

## Observed
- main/ and zone/english/ exist.
- 47 .iwd archives: all 47 open with Python's standard ZIP reader.
- 46,672 ZIP central-directory entries were indexed. No individual asset data was decompressed or CRC verified.
- 152 .ff zone files with recognized eight-byte signatures: IWffu100 (93), IWff0100 (59).
- No header or ZIP directory read errors.

This is not proof of valid asset payloads, maps, IW5 asset struct layouts or functional game execution.

## iOS shell
GameAssetInventory.swift now inspects a user-selected iOS Files folder in place. It reads at most four bytes from each IWD to check local ZIP header and eight bytes from each FF to recognize the outer signature. The iOS probe is intentionally less comprehensive than the Python Windows audit. Game files are neither copied to app storage nor committed to Git.

## Reproduce
python scripts/scan_iw5_layout.py "C:\Program Files (x86)\Call of Duty Modern Warfare 3"
