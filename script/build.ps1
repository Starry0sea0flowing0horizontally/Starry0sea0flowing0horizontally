param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

$ErrorActionPreference = "Stop"
$type = "debug"
$compiler = "msvc"

for ($i = 0; $i -lt $Arguments.Count; $i++) {
    $option = $Arguments[$i].ToLowerInvariant()
    if ($option -in @("-t", "--type")) {
        $i++
        if ($i -ge $Arguments.Count) { throw "缺少 -t/--type 的值（debug 或 release）。" }
        $type = $Arguments[$i].ToLowerInvariant()
    }
    elseif ($option -in @("-c", "--compiler")) {
        $i++
        if ($i -ge $Arguments.Count) { throw "缺少 -c/--compiler 的值（msvc 或 gcc）。" }
        $compiler = $Arguments[$i].ToLowerInvariant()
    }
    elseif ($option -in @("-h", "--help")) {
        Write-Host "用法: .\script\win64.ps1 [-t|--type debug|release] [-c|--compiler msvc|gcc]"
        exit 0
    }
    else {
        throw "未知参数: $($Arguments[$i])"
    }
}

if ($type -notin @("debug", "release")) {
    throw "无效构建类型 '$type'，可选值为 debug 或 release。"
}
if ($compiler -notin @("msvc", "gcc")) {
    throw "无效编译器 '$compiler'，可选值为 msvc 或 gcc。"
}

$projectDir = Split-Path $PSScriptRoot -Parent
$kitPattern = if ($compiler -eq "msvc") { "^msvc\d+_64$" } else { "^mingw\d*_64$" }
$qtRoot = $env:QT_ROOT
if (-not ($qtRoot -and (Test-Path (Join-Path $qtRoot "lib\cmake\Qt6\Qt6Config.cmake")))) {
    $searchRoot = if ($qtRoot -and (Test-Path $qtRoot)) { $qtRoot } else { "C:\Qt" }
    $qtRoot = Get-ChildItem $searchRoot -Directory -Recurse -Depth 5 -ErrorAction SilentlyContinue |
        Where-Object {
            $_.Name -match $kitPattern -and
            (Test-Path (Join-Path $_.FullName "lib\cmake\Qt6\Qt6Config.cmake"))
        } |
        Select-Object -First 1 -ExpandProperty FullName
}
if (-not $qtRoot) {
    throw "未找到匹配 $compiler 的 Qt 6 套件。请安装对应套件或设置 QT_ROOT。"
}
$env:QT_ROOT = $qtRoot

$cmakeType = if ($type -eq "debug") { "Debug" } else { "Release" }
$buildDir = Join-Path $projectDir "build\win64\$type\$compiler"

if ($compiler -eq "msvc") {
    $vsDevShell = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
    $vsInstanceId = "e389dc9f"
    if (-not (Test-Path $vsDevShell)) {
        throw "找不到 Visual Studio DevShell: $vsDevShell"
    }
    try {
        Import-Module $vsDevShell -ErrorAction Stop
        Enter-VsDevShell $vsInstanceId -SkipAutomaticLocation -DevCmdArguments "-arch=x64 -host_arch=x64"
    }
    catch {
        throw "Visual Studio 环境初始化失败: $_"
    }
}
else {
    $qtInstall = Split-Path (Split-Path $qtRoot -Parent) -Parent
    $qtTools = Join-Path $qtInstall "Tools"
    $mingwTool = Get-ChildItem $qtTools -Directory -Filter "mingw*" -ErrorAction SilentlyContinue |
        Where-Object { Test-Path (Join-Path $_.FullName "bin\g++.exe") } |
        Select-Object -First 1
    if (-not $mingwTool) {
        throw "找不到 Qt 配套的 MinGW 编译器（$qtTools）。"
    }
    $mingwBin = Join-Path $mingwTool.FullName "bin"
    $env:PATH = "$mingwBin;$(Join-Path $qtTools 'CMake_64\bin');$(Join-Path $qtTools 'Ninja');$env:PATH"
}

if (-not $env:VCPKG_ROOT -or
    -not (Test-Path (Join-Path $env:VCPKG_ROOT "scripts\buildsystems\vcpkg.cmake"))) {
    throw "请先设置 VCPKG_ROOT 指向 vcpkg 根目录。"
}

chcp.com 65001 > $null
[Console]::InputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$cmakeArgs = @(
    "-S", $projectDir,
    "-B", $buildDir,
    "-G", "Ninja",
    "-DCMAKE_BUILD_TYPE=$cmakeType",
    "-DCMAKE_PREFIX_PATH=$qtRoot"
)
if ($compiler -eq "gcc") {
    $cmakeArgs += @(
        "-DCMAKE_CXX_COMPILER=$(Join-Path $mingwBin 'g++.exe')",
        "-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic",
        "-DVCPKG_OVERLAY_TRIPLETS=$env:VCPKG_ROOT\triplets\community"
    )
}

Write-Host "Windows x64 | $cmakeType | $compiler | Qt: $qtRoot" -ForegroundColor Cyan
cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build $buildDir
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
