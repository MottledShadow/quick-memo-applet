param(
    [string]$Version = "1.0.0",
    [string]$BuildDir = "build\Desktop_Qt_6_11_1_MinGW_64_bit_Release",
    [string]$Configuration = "Release",
    [string]$PackageRoot = "build\release-package",
    [string]$WindeployQtPath = "",
    [string]$InnoSetupPath = ""
)

$ErrorActionPreference = "Stop"

function Resolve-RepoPath {
    param([string]$Path)
    if ([System.IO.Path]::IsPathRooted($Path)) {
        return [System.IO.Path]::GetFullPath($Path)
    }
    return [System.IO.Path]::GetFullPath((Join-Path $script:RepoRoot $Path))
}

function Assert-InWorkspace {
    param([string]$Path)
    $fullPath = [System.IO.Path]::GetFullPath($Path)
    if (-not $fullPath.StartsWith($script:RepoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to modify path outside workspace: $fullPath"
    }
    return $fullPath
}

function Find-WindeployQt {
    param([string]$ExplicitPath, [string]$ResolvedBuildDir)

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPath)) {
        $candidate = Resolve-RepoPath $ExplicitPath
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
        throw "windeployqt was not found at: $candidate"
    }

    $command = Get-Command windeployqt.exe -ErrorAction SilentlyContinue
    if ($command -ne $null) {
        return $command.Source
    }

    $cachePath = Join-Path $ResolvedBuildDir "CMakeCache.txt"
    if (Test-Path -LiteralPath $cachePath) {
        $prefixLine = Select-String -Path $cachePath -Pattern "^CMAKE_PREFIX_PATH:.*=(.+)$" | Select-Object -First 1
        if ($prefixLine -ne $null) {
            $prefix = $prefixLine.Matches[0].Groups[1].Value.Trim()
            $candidate = Join-Path $prefix "bin\windeployqt.exe"
            if (Test-Path -LiteralPath $candidate) {
                return $candidate
            }
        }
    }

    $commonRoots = @("C:\Qt\6.11.1\mingw_64\bin\windeployqt.exe", "C:\Qt\6.10.0\mingw_64\bin\windeployqt.exe")
    foreach ($candidate in $commonRoots) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw "windeployqt.exe was not found. Pass -WindeployQtPath or run from a Qt environment terminal."
}

function Find-InnoSetup {
    param([string]$ExplicitPath)

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPath)) {
        $candidate = Resolve-RepoPath $ExplicitPath
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
        throw "ISCC.exe was not found at: $candidate"
    }

    $command = Get-Command ISCC.exe -ErrorAction SilentlyContinue
    if ($command -ne $null) {
        return $command.Source
    }

    $candidates = @()
    if ($env:LOCALAPPDATA) {
        $candidates += Join-Path $env:LOCALAPPDATA "Programs\Inno\ISCC.exe"
        $candidates += Join-Path $env:LOCALAPPDATA "Programs\Inno Setup 6\ISCC.exe"
    }
    if ($env:ProgramFiles) {
        $candidates += Join-Path $env:ProgramFiles "Inno Setup 6\ISCC.exe"
    }
    $programFilesX86 = [Environment]::GetEnvironmentVariable("ProgramFiles(x86)")
    if ($programFilesX86) {
        $candidates += Join-Path $programFilesX86 "Inno Setup 6\ISCC.exe"
    }

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw "ISCC.exe was not found. Install Inno Setup 6 or pass -InnoSetupPath."
}

function Remove-PathIfExists {
    param([string]$Path, [switch]$Recurse)

    if (-not (Test-Path -LiteralPath $Path)) {
        return
    }

    $resolved = Assert-InWorkspace (Resolve-Path -LiteralPath $Path).Path
    if ($Recurse) {
        Remove-Item -LiteralPath $resolved -Recurse -Force
    } else {
        Remove-Item -LiteralPath $resolved -Force
    }
}

$script:RepoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
Set-Location $script:RepoRoot

$resolvedBuildDir = Resolve-RepoPath $BuildDir
$resolvedPackageRoot = Assert-InWorkspace (Resolve-RepoPath $PackageRoot)
$packageName = "QuickMemoApplet-v$Version-win64"
$deployDir = Assert-InWorkspace (Join-Path $resolvedPackageRoot $packageName)
$zipPath = Assert-InWorkspace (Join-Path $resolvedPackageRoot "$packageName.zip")
$hashPath = Assert-InWorkspace (Join-Path $resolvedPackageRoot "$packageName-SHA256SUMS.txt")
$installerScript = Join-Path $script:RepoRoot "installer\quick-memo-applet.iss"
$setupPath = Assert-InWorkspace (Join-Path $resolvedPackageRoot "$packageName-setup.exe")

if (-not (Test-Path -LiteralPath $resolvedBuildDir)) {
    throw "Release build directory was not found: $resolvedBuildDir"
}

New-Item -ItemType Directory -Force -Path $resolvedPackageRoot | Out-Null

Write-Host "Building $Configuration target..."
& cmake --build $resolvedBuildDir --config $Configuration

$exePath = Join-Path $resolvedBuildDir "Quick_Memo_Applet.exe"
if (-not (Test-Path -LiteralPath $exePath)) {
    throw "Built executable was not found: $exePath"
}

Remove-PathIfExists $deployDir -Recurse
Remove-PathIfExists $zipPath
Remove-PathIfExists $hashPath
Remove-PathIfExists $setupPath

New-Item -ItemType Directory -Force -Path $deployDir | Out-Null
Copy-Item -LiteralPath $exePath -Destination $deployDir
Copy-Item -LiteralPath "README.md", "LICENSE", "THIRD_PARTY_NOTICES.md" -Destination $deployDir

$windeployQt = Find-WindeployQt -ExplicitPath $WindeployQtPath -ResolvedBuildDir $resolvedBuildDir
Write-Host "Deploying Qt runtime with $windeployQt..."
& $windeployQt --release (Join-Path $deployDir "Quick_Memo_Applet.exe")

Write-Host "Creating zip package..."
Compress-Archive -LiteralPath $deployDir -DestinationPath $zipPath -CompressionLevel Optimal

$iscc = Find-InnoSetup -ExplicitPath $InnoSetupPath
Write-Host "Creating installer with $iscc..."
& $iscc "/DAppVersion=$Version" "/DSourceDir=$deployDir" "/DOutputDir=$resolvedPackageRoot" $installerScript

if (-not (Test-Path -LiteralPath $setupPath)) {
    throw "Installer was not created: $setupPath"
}

$zipHash = Get-FileHash -Algorithm SHA256 -LiteralPath $zipPath
$setupHash = Get-FileHash -Algorithm SHA256 -LiteralPath $setupPath
$hashLines = @(
    "$($zipHash.Hash)  $([System.IO.Path]::GetFileName($zipPath))",
    "$($setupHash.Hash)  $([System.IO.Path]::GetFileName($setupPath))"
)
$hashLines | Set-Content -LiteralPath $hashPath -Encoding UTF8

Write-Host ""
Write-Host "Package complete:"
Write-Host "  $zipPath"
Write-Host "  $setupPath"
Write-Host "  $hashPath"
Write-Host ""
Write-Host "SHA256:"
$hashLines | ForEach-Object { Write-Host "  $_" }
