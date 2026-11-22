# ============================================================================
#  花至荼蘼（同人视觉小说）—— CMake 构建配置
#  纯标准 C++23，零第三方依赖，跨平台（Windows / Linux / macOS）
# ============================================================================
cmake_minimum_required(VERSION 4.3)
project(huazhitumi VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(huazhitumi
  src/main.cpp
  src/input.cpp
  src/renderer.cpp
  src/save.cpp
  src/status.cpp
  src/game.cpp
  src/map.cpp
  src/prologue.cpp
)

target_include_directories(huazhitumi PRIVATE src)

# 打字机演出使用 std::this_thread::sleep_for，需链接线程库（Linux/macOS 为 pthread）
find_package(Threads REQUIRED)
target_link_libraries(huazhitumi PRIVATE Threads::Threads)

if(MSVC)
  # /utf-8：源码与执行字符集均为 UTF-8（中文正确显示）
  target_compile_options(huazhitumi PRIVATE /utf-8 /W3)
  target_compile_definitions(huazhitumi PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(huazhitumi PRIVATE -Wall -Wextra)
  # 确保中文源码以 UTF-8 读取、字符串以 UTF-8 编码
  target_compile_options(huazhitumi PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

install(TARGETS huazhitumi
  RUNTIME DESTINATION .
)
install(DIRECTORY assets/
  DESTINATION assets
)

set(CPACK_PACKAGE_NAME "huazhitumi")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "zgteam233")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "花至荼蘼 - 同人视觉小说")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

if(MSVC)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_PACKAGE_FILE_NAME "huazhitumi-${PROJECT_VERSION}-win64-msvc")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_PACKAGE_FILE_NAME "huazhitumi-${PROJECT_VERSION}-anylinux64-gcc")
else()
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_PACKAGE_FILE_NAME "huazhitumi-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
endif()

include(CPack)
