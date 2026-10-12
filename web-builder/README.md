# Browser Builder — design only

User proposal: a web interface authenticates with their own GitHub account, uploads three approved build files directly to a private repository, then starts GitHub Actions.

No functional website has yet been written or deployed.

Security requirements:
- Client-to-GitHub transfer only, no server-side proxy of files.
- GitHub OAuth/PKCE or another supported low-scope login mechanism; never store access tokens in repo or web assets.
- Confirm each upload and restrict to user-approved files.
- Do not claim a server stores nothing until hosting paths and analytics/logging behavior are verified.
- No redistribution of third-party game code, shaders or assets without authorization.
