# Publish this new project to GitHub

Target GitHub repository: https://github.com/R347H4CK3R/MW3-iOS-Native

Local repo: C:\\Users\\Chris\\Documents\\MW3-iOS-Native

The connected GitHub integration can modify existing repositories but does not offer a "create repository" operation. The PC currently has Git but no GitHub CLI (gh) on PATH.

Create a new **private, empty** GitHub repository named `MW3-iOS-Native`, owned by `R347H4CK3R`, with NO generated README/.gitignore/license (they already exist locally).

Then in PowerShell:

    cd 'C:\\Users\\Chris\\Documents\\MW3-iOS-Native'
    git remote add origin https://github.com/R347H4CK3R/MW3-iOS-Native.git
    git push -u origin main

Alternatively, once the repository exists, the connected GitHub integration can upload source files.

Do not commit original MW3 binaries or assets. No playable IPA currently exists.
