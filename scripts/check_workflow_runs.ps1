# Lists GitHub workflow runs using credentials already on this PC.
# Access token remains in process memory, never in logs/files.
$ErrorActionPreference = 'Stop'
$env:GCM_INTERACTIVE = 'Never'
$token = $null
$raw = $null
try {
    $raw = ("protocol=https" + [Environment]::NewLine + "host=github.com" + [Environment]::NewLine + [Environment]::NewLine) | git -c credential.interactive=never credential fill 2>$null
    if ($LASTEXITCODE -ne 0) { Write-Output "GitHub credentials unavailable"; exit 2 }
    $line = @($raw | Where-Object { $_ -match '^password=' } | Select-Object -First 1)
    if ($line.Count -eq 0) { Write-Output "GitHub token unavailable"; exit 2 }
    $token = $line[0].Substring(9)
    $response = Invoke-RestMethod -Uri 'https://api.github.com/repos/R347H4CK3R/MW3-iOS-Native/actions/runs?per_page=8' -Headers @{
        Authorization = ('Bearer ' + $token)
        Accept = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'MW3-iOS-Native-CI-Status'
    }
    foreach ($run in $response.workflow_runs) {
        Write-Output ($run.name + ' | ' + $run.status + ' | ' + $run.conclusion + ' | ' + $run.html_url)
    }
}
finally {
    Remove-Variable token,raw,line -ErrorAction SilentlyContinue
    Remove-Item Env:GCM_INTERACTIVE -ErrorAction SilentlyContinue
}
