# Dispatch the manually gated iOS shell build using already stored Git credentials.
# This may consume private-repository macOS GitHub Actions minutes.
$ErrorActionPreference = 'Stop'
$env:GCM_INTERACTIVE = 'Never'
$token = $null
$raw = $null
try {
    $raw = ("protocol=https" + [Environment]::NewLine + "host=github.com" + [Environment]::NewLine + [Environment]::NewLine) | git -c credential.interactive=never credential fill 2>$null
    if ($LASTEXITCODE -ne 0) { throw 'GitHub credentials unavailable' }
    $line = @($raw | Where-Object { $_ -match '^password=' } | Select-Object -First 1)
    if ($line.Count -eq 0) { throw 'GitHub credential has no token' }
    $token = $line[0].Substring(9)
    $headers = @{
        Authorization = ('Bearer ' + $token)
        Accept = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'MW3-iOS-Native-Shell-Build'
    }
    $uri = 'https://api.github.com/repos/R347H4CK3R/MW3-iOS-Native/actions/workflows/windows-iwd-probe.yml/dispatches'
    Invoke-RestMethod -Uri $uri -Method Post -ContentType 'application/json' -Headers $headers -Body '{"ref":"main"}' | Out-Null
    Write-Output 'DISPATCH_ACCEPTED: windows-iwd-probe.yml at main'
}
catch {
    Write-Output ('DISPATCH_FAILED: ' + $_.Exception.Message)
    exit 1
}
finally {
    Remove-Variable token,raw,line -ErrorAction SilentlyContinue
    Remove-Item Env:GCM_INTERACTIVE -ErrorAction SilentlyContinue
}

