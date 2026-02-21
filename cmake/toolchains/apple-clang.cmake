# SPDX-License-Identifier: Apache-2.0
# Apple Clang toolchain (macOS)
# Ensures CMake never selects clang-cl (MSVC emulation) on macOS.

if(NOT APPLE)
  message(FATAL_ERROR "apple-clang.cmake is intended for macOS only")
endif()

execute_process(
  COMMAND xcrun -find clang
  OUTPUT_VARIABLE _clang
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
execute_process(
  COMMAND xcrun -find clang++
  OUTPUT_VARIABLE _clangxx
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
execute_process(
  COMMAND xcrun -find ar
  OUTPUT_VARIABLE _ar
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
execute_process(
  COMMAND xcrun -find ranlib
  OUTPUT_VARIABLE _ranlib
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
execute_process(
  COMMAND xcrun --sdk macosx --show-sdk-path
  OUTPUT_VARIABLE _sdk
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

if(NOT _clang OR NOT EXISTS "${_clang}")
  message(FATAL_ERROR "xcrun could not find clang; install Xcode command line tools")
endif()
if(NOT _clangxx OR NOT EXISTS "${_clangxx}")
  message(FATAL_ERROR "xcrun could not find clang++; install Xcode command line tools")
endif()
if(NOT _ar OR NOT EXISTS "${_ar}")
  message(FATAL_ERROR "xcrun could not find ar; install Xcode command line tools")
endif()
if(NOT _ranlib OR NOT EXISTS "${_ranlib}")
  message(FATAL_ERROR "xcrun could not find ranlib; install Xcode command line tools")
endif()

set(CMAKE_C_COMPILER "${_clang}" CACHE STRING "Apple clang" FORCE)
set(CMAKE_CXX_COMPILER "${_clangxx}" CACHE STRING "Apple clang++" FORCE)
set(CMAKE_AR "${_ar}" CACHE STRING "Apple ar" FORCE)
set(CMAKE_RANLIB "${_ranlib}" CACHE STRING "Apple ranlib" FORCE)

if(_sdk)
  set(CMAKE_OSX_SYSROOT "${_sdk}" CACHE PATH "macOS SDK" FORCE)
endif()

message(STATUS "Apple clang toolchain: ${_clangxx}")
