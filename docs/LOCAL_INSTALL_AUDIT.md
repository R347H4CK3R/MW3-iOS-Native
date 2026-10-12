# Read-only Windows MW3 installation inventory

Inspected source folder on connected GamingLaptop:
C:\\Program Files (x86)\\Call of Duty Modern Warfare 3

Observed:
- `iw5sp.exe` and `iw5mp.exe` at installation root (Windows PE executables, NOT WebAssembly).
- `main` contains 35 numbered `iw_00.iwd` through `iw_34.iwd`, plus 12 numbered `localized_english_iw00.iwd` through `localized_english_iw11.iwd`.
- `zone\\english` contains 152 `.ff` files. Sample entries: `code_pre_gfx.ff`, `code_post_gfx_mp.ff`, `common_mp.ff`, `common_specialops.ff`, `common_survival.ff`, `mp_dome.ff`.
- `zone\\dlc`, `main\\video` and `main\\demo` directories exist.

The inventory only examines filenames and magic bytes of one executable; no original game archives, shaders, maps, executables, or media were copied into or committed to Git.

The presence of .ff and .iwd data does not mean any iOS engine, shader converter or asset decoder exists. This inventory is a guide for future lawful on-device loading research.
