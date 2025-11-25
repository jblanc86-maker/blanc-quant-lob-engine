cmake_minimum_required(VERSION 3.20)
project(blanc_lob_engine VERSION 0.9 LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

set(CMAKE_CXX_STANDARD 20)

# Good for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings: strict in CI, softer locally
if (NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Optional: only make warnings fatal in CI
if (DEFINED ENV{CI})
  add_compile_options(-Werror)
endif()

# Linux-only hardening (Release)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_BUILD_TYPE MATCHES "Rel")
  include(CheckCXXCompilerFlag)
  foreach(flag -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE)
    string(REPLACE "-" "_" safe "${flag}")
    check_cxx_compiler_flag(${flag} HAS_${safe})
    if (HAS_${safe})
      add_compile_options(${flag})
    endif()
  endforeach()
  add_link_options(-pie)
  # If your linker supports it, consider:
  # add_link_options(-Wl,-z,relro,-z,now)
endif()

# Optional sanitizers for Debug builds
option(ENABLE_SANITIZERS "Enable ASAN/UBSAN" OFF)
if (ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# Put binaries in build/bin for consistent paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS program_options)


add_executable(replay
  src/replay.cpp
  src/breaker.cpp
  src/telemetry.cpp
)
target_include_directories(replay PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(replay PRIVATE -O3 -march=native)
target_link_libraries(replay PRIVATE Boost::program_options)

enable_testing()

add_test(NAME replay_help COMMAND $<TARGET_FILE:replay> --help)
set_tests_properties(replay_help PROPERTIES PASS_REGULAR_EXPRESSION "Blanc LOB Engine")

add_test(NAME replay_default_run COMMAND $<TARGET_FILE:replay>)
set_tests_properties(replay_default_run PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
