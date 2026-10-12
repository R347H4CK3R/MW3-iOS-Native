$ErrorActionPreference = 'Stop'
$env:GCM_INTERACTIVE = 'Never'
$raw = $null
$token = $null
try {
    $raw = ("protocol=https" + [Environment]::NewLine + "host=github.com" + [Environment]::NewLine + [Environment]::NewLine) | git -c credential.interactive=never credential fill 2>$null
    if ($LASTEXITCODE -ne 0) { Write-Output 'AUTH_NOT_AVAILABLE'; exit 2 }
    $line = @($raw | Where-Object { $_ -match '^password=' } | Select-Object -First 1)
    if ($line.Count -eq 0) { Write-Output 'AUTH_NOT_AVAILABLE'; exit 2 }
    $token = $line[0].Substring(9)
    $headers = @{
        Authorization = ('Bearer ' + $token)
        Accept = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'MW3-iOS-Native-Proof'
    }
    Invoke-RestMethod -Method Post -Uri 'https://api.github.com/repos/R347H4CK3R/MW3-iOS-Native/actions/workflows/wasm-arm64-proof.yml/dispatches' -Headers $headers -ContentType 'application/json' -Body '{"ref":"main"}' | Out-Null
    Write-Output 'DISPATCHED toy Wasm-to-ARM64 proof GitHub workflow'
}
catch {
    $status = 'unknown'
    if ($null -ne $_.Exception.Response) { $status = [string][int]$_.Exception.Response.StatusCode }
    Write-Output ('DISPATCH_FAILED HTTP=' + $status)
    exit 1
}
finally {
    Remove-Variable token,raw,line -ErrorAction SilentlyContinue
    Remove-Item Env:GCM_INTERACTIVE -ErrorAction SilentlyContinue
}
