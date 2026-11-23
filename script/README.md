# 构建脚本

脚本统一通过 `-t` / `--type` 选择构建类型：`debug` 或 `release`，默认 `debug`。Debug 和 Release 输出到不同目录，不会互相覆盖。

| 脚本 | 平台 / 编译器 | 输出目录 |
| --- | --- | --- |
| `win64.ps1` | Windows x64，MSVC 或 GCC/MinGW | `build/win64/<type>/<compiler>/` |
| `anylinux64` | Linux / WSL shell | `build/anylinux64/<type>/` |

## Windows x64

默认使用 MSVC 和 Debug：

```powershell
.\script\win64.ps1
```

选择构建类型和编译器：

```powershell
.\script\win64.ps1 -t release -c msvc
.\script\win64.ps1 --type debug --compiler gcc
```

`-c` / `--compiler` 可选 `msvc`（默认）或 `gcc`。MSVC 模式会按原构建流程初始化 Visual Studio Developer 环境；GCC 模式使用 Qt 安装目录下的 MinGW 工具链。脚本会查找匹配的 Qt 6 套件，也可通过 `QT_ROOT` 指定套件目录。两种模式都需要设置 `VCPKG_ROOT`。

## Linux / WSL

在 Linux 或 WSL shell 中运行无后缀脚本：

```bash
bash script/anylinux64
bash script/anylinux64 --type release
```

Linux 构建需要 Linux 版 vcpkg，并在 shell 环境中设置 `VCPKG_ROOT`。如需 Qt 桌面界面，可安装 Qt 6 Widgets，或将 `QT_ROOT` 指向 Linux Qt 套件（例如 `/mnt/c/qt/6.12.0/gcc_64`）；脚本会通过 CMake 自动检测。
