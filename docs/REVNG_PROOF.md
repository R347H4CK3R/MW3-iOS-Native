# rev.ng actual decompilation experiment

Runs via manual GitHub Actions workflow: `.github/workflows/revng-real-pe.yml`.

Official documentation provides `docker run --rm -it revng/revng` and `revng quick artifact emit-c-as-single-file INPUT OUTPUT`.

A synthetic PE32 test fixture is created inside the GitHub runner from the project code. The workflow runs **rev.ng itself** to produce a decompiled C artifact.

Acceptance: non-empty output with successful exit status. This is deliberately a decompilation proof, not executable equivalence or ARM64 iOS compilation.

MW3 game binaries are NOT sent to GitHub.

Documentation: https://docs.rev.ng/user-manual/initial-setup/ and https://docs.rev.ng/references/cli/revng-quick/artifact/
