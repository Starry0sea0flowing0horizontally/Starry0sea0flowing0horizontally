# ============================================================================
#  花至荼蘼（同人视觉小说）—— CMake 构建配置
#  C++23，跨平台（Windows / Linux / macOS）
# ============================================================================
cmake_minimum_required(VERSION 4.3)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "vcpkg toolchain file")
endif()

project(huazhitumi VERSION 1.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HZ_ENABLE_QT_GUI "Build the Qt desktop interface when Qt 6 is available" ON)

if(HZ_ENABLE_QT_GUI)
  file(TO_CMAKE_PATH "$ENV{QT_ROOT}" _qt_root_hint)
  file(TO_CMAKE_PATH "$ENV{QT_DIR}" _qt_dir_hint)
  set(_qt_hints "${_qt_root_hint}" "${_qt_dir_hint}")
  if(MSVC)
    file(GLOB _qt_candidates LIST_DIRECTORIES true "C:/Qt/*/msvc*_64")
  elseif(WIN32)
    file(GLOB _qt_candidates LIST_DIRECTORIES true "C:/Qt/*/mingw*_64")
  else()
    file(GLOB _qt_candidates LIST_DIRECTORIES true
      "/mnt/c/qt/*/gcc_64"
    )
  endif()
  list(APPEND _qt_hints ${_qt_candidates})
  find_package(Qt6 QUIET COMPONENTS Widgets HINTS ${_qt_hints})
endif()

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

target_include_directories(huazhitumi PRIVATE include)

if(Qt6_FOUND)
  target_sources(huazhitumi PRIVATE src/qt_frontend.cpp)
  target_compile_definitions(huazhitumi PRIVATE HZ_QT_FRONTEND)
  target_link_libraries(huazhitumi PRIVATE Qt6::Widgets)
  if(WIN32)
    set_target_properties(huazhitumi PROPERTIES WIN32_EXECUTABLE TRUE)
    find_program(QT_DEPLOY_TOOL windeployqt HINTS "${Qt6_DIR}/../../../bin" REQUIRED)
    set(_qt_deploy_options --no-translations)
    if(MINGW)
      list(APPEND _qt_deploy_options --compiler-runtime)
    endif()
    add_custom_command(TARGET huazhitumi POST_BUILD
      COMMAND "${QT_DEPLOY_TOOL}" ${_qt_deploy_options} "$<TARGET_FILE:huazhitumi>"
      VERBATIM
    )
  endif()
  message(STATUS "Qt 6 desktop interface enabled")
else()
  message(STATUS "Qt 6 not found; building the terminal interface")
endif()

# 打字机演出使用 std::this_thread::sleep_for，需链接线程库（Linux/macOS 为 pthread）
find_package(Threads REQUIRED)
find_package(LibArchive REQUIRED)
target_link_libraries(huazhitumi PRIVATE Threads::Threads LibArchive::LibArchive)

include(CTest)
if(BUILD_TESTING)
  add_executable(save_format_test tests/save_format_test.cpp src/save.cpp)
  target_include_directories(save_format_test PRIVATE include)
  target_link_libraries(save_format_test PRIVATE LibArchive::LibArchive)
  if(MSVC)
    target_compile_options(save_format_test PRIVATE /utf-8 /W3)
    target_compile_definitions(save_format_test PRIVATE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX
    )
  else()
    target_compile_options(save_format_test PRIVATE -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
  endif()
  add_test(NAME save_format COMMAND save_format_test)
endif()

if(MSVC)
  # /utf-8：源码与执行字符集均为 UTF-8（中文正确显示）
  target_compile_options(huazhitumi PRIVATE /utf-8 /W3)
  target_compile_definitions(huazhitumi PRIVATE
          _CRT_SECURE_NO_WARNINGS
          NOMINMAX
  )
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
