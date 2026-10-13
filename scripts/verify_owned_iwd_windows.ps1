<#
Downloads the independent native IWD checker from our private GitHub Actions
workflow to this PC, then checks one user-owned IWD IN PLACE.
Never uploads/copies any MW3 archive or entry data.
#>
param(
  [Parameter(Mandatory=$true)][long]$RunId,
  [string]$ArchivePath = 'C:\Program Files (x86)\Steam\steamapps\common\Call of Duty Modern Warfare 3\main\iw_00.iwd'
)
$ErrorActionPreference = 'Stop'
$env:GCM_INTERACTIVE = 'Never'
$raw = $null
$token = $null
try {
  if (-not (Test-Path -LiteralPath $ArchivePath -PathType Leaf)) {
    throw "MW3 archive unavailable at selected path (no game data changed): $ArchivePath"
  }
  $raw = ("protocol=https" + [Environment]::NewLine +
          "host=github.com" + [Environment]::NewLine +
          [Environment]::NewLine) | git -c credential.interactive=never credential fill 2>$null
  if ($LASTEXITCODE -ne 0) { throw 'No saved GitHub credentials' }
  $line = @($raw | Where-Object { $_ -match '^password=' } | Select-Object -First 1)
  if ($line.Count -ne 1) { throw 'GitHub credential unavailable' }
  $token = $line[0].Substring(9)
  $headers = @{
    Authorization = 'Bearer ' + $token
    Accept = 'application/vnd.github+json'
    'X-GitHub-Api-Version' = '2022-11-28'
    'User-Agent' = 'MW3-iOS-Native-Local-IWD-Validator'
  }
  $base = 'https://api.github.com/repos/R347H4CK3R/MW3-iOS-Native'
  $listing = Invoke-RestMethod -Uri "$base/actions/runs/$RunId/artifacts" -Headers $headers
  $artifact = @($listing.artifacts | Where-Object {
      $_.name -eq 'IWD-Probe-Windows-x64-No-Game-Files' -and (-not $_.expired)
  } | Select-Object -First 1)
  if ($artifact.Count -ne 1) { throw "Artifact not yet available for run $RunId" }

  $destination = Join-Path (Split-Path -Parent $PSScriptRoot) 'local-fixtures\windows-reader'
  New-Item -ItemType Directory -Force -Path $destination | Out-Null
  $archive = Join-Path $destination 'reader-artifact.zip'
  Invoke-WebRequest -Uri "$base/actions/artifacts/$($artifact[0].id)/zip" -Headers $headers -OutFile $archive
  Expand-Archive -LiteralPath $archive -DestinationPath $destination -Force
  $exe = Join-Path $destination 'iwd_probe_win64.exe'
  if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw 'Windows reader EXE absent from artifact' }
  Write-Output ('Native reader SHA256: ' + (Get-FileHash -Algorithm SHA256 $exe).Hash)
  Write-Output 'Opening installed .iwd archive read-only, not extracting to disk.'
  & $exe $ArchivePath
  if ($LASTEXITCODE -ne 0) { throw "IWD validation failed with exit code $LASTEXITCODE" }
  Write-Output 'LOCAL_NATIVE_IWD_CHECK_PASSED'
  $archiveFolder = Split-Path -Parent $ArchivePath
  $candidateArchives = @(Get-ChildItem -LiteralPath $archiveFolder -Filter '*.iwd' -File)
  $passed = 0
  $noSample = 0
  $failed = @()
  foreach($candidate in $candidateArchives) {
    $result = @(& $exe $candidate.FullName 2>&1)
    $status = $LASTEXITCODE
    if($status -eq 0) {
      $passed += 1
      if([string]::Join(' ',[string[]]$result) -match 'IWD_NO_SMALL_READABLE_SAMPLE') {
        $noSample += 1
      }
    } else {
      $failed += ($candidate.Name + ': ' + [string]::Join(' ',[string[]]$result))
    }
  }
  Write-Output ("IWD_ARCHIVES_PASSED $passed/$($candidateArchives.Count)")
  Write-Output ("IWD_ARCHIVES_WITHOUT_SMALL_SAMPLE $noSample")
  if($failed.Count -gt 0) {
    $failed | ForEach-Object { Write-Output $_ }
    throw "$($failed.Count) archive validation failures"
  }
}
finally {
  Remove-Variable raw,token,line,headers -ErrorAction SilentlyContinue
  Remove-Item Env:GCM_INTERACTIVE -ErrorAction SilentlyContinue
}
