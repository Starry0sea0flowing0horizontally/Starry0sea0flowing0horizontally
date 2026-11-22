#!/usr/bin/env bash

printf '\033[36m\nBuild 花至荼蘼 · 同人视觉小说\nStarry Sea Flowing Horizontally\033[0m\n'

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

printf '\033[33m\n[1/2] 清理构建文件...\nCleaning build files...\033[0m\n'
if [[ -d "$BUILD_DIR" ]]; then
    rm -rf -- "$BUILD_DIR"
else
    printf '\033[32m\n目录不存在. 跳过清理: %s\nDir not found. Skipping cleanup.\033[0m\n' "$BUILD_DIR"
fi

printf '\033[33m\n[2/2] 构建中...\nBuilding program...\033[0m\n'
mkdir -p -- "$BUILD_DIR"
cd -- "$BUILD_DIR" || exit $?

if cmake .. -G Ninja; then
    :
else
    status=$?
    printf '\033[31mBuild failed with exit code %s\033[0m\n' "$status" >&2
    exit "$status"
fi

if cmake --build .; then
    :
else
    status=$?
    printf '\033[31mBuild failed with exit code %s\033[0m\n' "$status" >&2
    exit "$status"
fi

exit 0
