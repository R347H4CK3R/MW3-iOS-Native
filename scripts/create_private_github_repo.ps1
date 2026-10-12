# Create private GitHub repository using PC's existing Git Credential Manager.
# Access token is used only in process memory and is not printed or persisted.
$ErrorActionPreference = 'Stop'
$env:GCM_INTERACTIVE = 'Never'
$credentialText = $null
$token = $null
try {
    $request = "protocol=https" + [Environment]::NewLine + "host=github.com" + [Environment]::NewLine + [Environment]::NewLine
    $credentialText = $request | git -c credential.interactive=never credential fill 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Output 'AUTH_NOT_AVAILABLE: no non-interactive GitHub credentials.'
        exit 2
    }
    $tokenLine = @($credentialText | Where-Object { $_ -match '^password=' } | Select-Object -First 1)
    if ($tokenLine.Count -eq 0) {
        Write-Output 'AUTH_NOT_AVAILABLE: Git credentials did not include an access token.'
        exit 2
    }
    $token = $tokenLine[0].Substring('password='.Length)
    $headers = @{
        'Authorization' = 'Bearer ' + $token
        'Accept' = 'application/vnd.github+json'
        'X-GitHub-Api-Version' = '2022-11-28'
        'User-Agent' = 'MW3-iOS-Native-Bootstrap'
    }
    $body = @{
        name = 'MW3-iOS-Native'
        description = 'Private ARM64 iOS MW3 portability research and controller bridge scaffold; no game assets'
        private = $true
        auto_init = $false
        has_issues = $true
    } | ConvertTo-Json -Compress
    try {
        $created = Invoke-RestMethod -Uri 'https://api.github.com/user/repos' -Method Post -Headers $headers -ContentType 'application/json' -Body $body
        if ($created.full_name -ne 'R347H4CK3R/MW3-iOS-Native' -or -not $created.private) {
            Write-Output 'CREATE_UNEXPECTED: Repository identity or privacy mismatch.'
            exit 1
        }
        Write-Output ('CREATED ' + $created.full_name + ' private=' + $created.private + ' url=' + $created.html_url)
    }
    catch {
        $status = 'unknown'
        if ($null -ne $_.Exception.Response) {
            $status = [string][int]$_.Exception.Response.StatusCode
        }
        Write-Output ('CREATE_FAILED HTTP=' + $status + ' (token scope, authorization or existing repository)')
        exit 1
    }
}
finally {
    Remove-Variable token,credentialText,tokenLine -ErrorAction SilentlyContinue
    Remove-Item Env:GCM_INTERACTIVE -ErrorAction SilentlyContinue
}
