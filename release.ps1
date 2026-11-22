# ==========================================
# Build 花至荼蘼 · 同人视觉小说
# Starry Sea Flowing Horizontally
# ==========================================

$ErrorActionPreference = "Stop"

# Visual Studio Developer PowerShell
$vsDevShell = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
$vsInstanceId = "e389dc9f"

Write-Host "`n初始化 Visual Studio 开发环境..." -ForegroundColor Cyan

if (-not (Test-Path $vsDevShell)) {
    Write-Host "找不到 Visual Studio DevShell: $vsDevShell" -ForegroundColor Red
    exit 1
}

try {
    Import-Module $vsDevShell -ErrorAction Stop

    Enter-VsDevShell $vsInstanceId `
        -SkipAutomaticLocation `
        -DevCmdArguments "-arch=x64 -host_arch=x64"
}
catch {
    Write-Host "Visual Studio 环境初始化失败: $_" -ForegroundColor Red
    exit 1
}

# UTF-8
chcp.com 65001 > $null
[Console]::InputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "`n==========================================" -ForegroundColor Cyan
Write-Host "Build 花至荼蘼 · 同人视觉小说" -ForegroundColor Cyan
Write-Host "Starry Sea Flowing Horizontally" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# Build directory
$projectDir = $PSScriptRoot
$buildDir = Join-Path $projectDir "build"

# 1. Clean
Write-Host "`n[1/3] 清理构建文件... / Cleaning build files..." -ForegroundColor Yellow

if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
else {
    Write-Host "目录不存在，跳过清理: $buildDir" -ForegroundColor Green
}

# 2. Configure
Write-Host "`n[2/3] CMake 配置... / Configuring..." -ForegroundColor Yellow

New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Set-Location $buildDir

cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake 配置失败，退出代码: $LASTEXITCODE" -ForegroundColor Red
    Set-Location $projectDir
    exit $LASTEXITCODE
}

# 3. Build
Write-Host "`n[3/3] 开始构建... / Building..." -ForegroundColor Yellow

cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    Set-Location $projectDir
    exit $LASTEXITCODE
}

Set-Location $projectDir

Write-Host "`n==========================================" -ForegroundColor Green
Write-Host "构建成功！Build completed successfully!" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Green

exit 0
