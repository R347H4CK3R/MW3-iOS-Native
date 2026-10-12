# Repository publishing status

The project has been created as a **private GitHub repository**:
https://github.com/R347H4CK3R/MW3-iOS-Native

Local Windows repository:
C:\\Users\\Chris\\Documents\\MW3-iOS-Native

The local `main` branch tracks `origin/main`. Updates can be made and pushed with:

    git -C "C:\\Users\\Chris\\Documents\\MW3-iOS-Native" add -A
    git -C "C:\\Users\\Chris\\Documents\\MW3-iOS-Native" commit -m "Describe the change"
    git -C "C:\\Users\\Chris\\Documents\\MW3-iOS-Native" push origin main

Do not commit source files from the original MW3 installation, proprietary game binaries, licensed assets or account tokens. Never confuse the iOS input/Metal probe shell with a working MW3 port.

CI:
- `.github/workflows/validate.yml` runs controller bridge tests and Python readiness checks on every push.
- `.github/workflows/ios-shell-smoke.yml` is opt-in/manual and uses a potentially billable macOS runner. It has not been run.
