# Validate user-owned IWD archives without uploading them

The Windows IWD tool is compiled in the private repository by
.github/workflows/windows-iwd-probe.yml. It packages only the test
executable and zlib runtime, never an MW3 archive.

From the Windows PC, after the workflow has passed, run:

    powershell -NoProfile -ExecutionPolicy Bypass -File scripts/verify_owned_iwd_windows.ps1 -RunId 38333737844

The PowerShell script downloads the private artifact using Git
credentials already present on that PC, and opens all IWD archives
from the selected folder read-only. To change the folder, pass
-ArchivePath "D:\your MW3 folder\main\iw_00.iwd".

The validator emits index metadata and sample CRC status only.
It never outputs or stores original texture/map/shader bytes.
