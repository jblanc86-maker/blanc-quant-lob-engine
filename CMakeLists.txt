cmake_minimum_required(VERSION 3.20)
project(blanc_lob_engine VERSION 0.9 LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

# Option to build benchmarks
option(BUILD_BENCHMARKS "Build microbenchmark suite" ON)

# Add bench subdirectory if enabled
if (BUILD_BENCHMARKS)
  add_subdirectory(bench)
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
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(replay
  src/replay.cpp
  src/breaker.cpp
  src/telemetry.cpp
)
target_include_directories(replay PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(replay PRIVATE -O3 -march=native)
target_link_libraries(replay PRIVATE Boost::program_options)

add_executable(gen_synth
  tools/gen_synth.cpp
)
target_compile_options(gen_synth PRIVATE -O3 -march=native)

set(GOLDEN_DIR ${CMAKE_SOURCE_DIR}/data/golden)
set(GOLDEN_BIN ${GOLDEN_DIR}/itch_1m.bin)

add_custom_command(
  OUTPUT ${GOLDEN_BIN}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${GOLDEN_DIR}
  COMMAND $<TARGET_FILE:gen_synth> --count 1000000 --out ${GOLDEN_BIN}
  DEPENDS gen_synth
  COMMENT "Generating deterministic ITCH sample at ${GOLDEN_BIN}"
)

add_custom_target(golden_sample ALL
  DEPENDS ${GOLDEN_BIN}
)

# -----------------
# Testing (CTest)
# -----------------
include(CTest)
if (BUILD_TESTING)
  enable_testing()
  if (EXISTS ${CMAKE_SOURCE_DIR}/tests/test_breaker.cpp)
    add_executable(test_breaker
      tests/test_breaker.cpp
      src/breaker.cpp
    )
    target_include_directories(test_breaker PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME breaker_basic COMMAND test_breaker)
  else()
    message(STATUS "Skipping test_breaker: tests/test_breaker.cpp not present")
  endif()

  if (EXISTS ${CMAKE_SOURCE_DIR}/tests/test_telemetry.cpp)
    add_executable(test_telemetry
      tests/test_telemetry.cpp
      src/telemetry.cpp
      src/breaker.cpp
    )
    target_include_directories(test_telemetry PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME telemetry_io COMMAND test_telemetry)
  endif()

  if (EXISTS ${CMAKE_SOURCE_DIR}/tests/test_golden.cpp)
    add_executable(test_golden
      tests/test_golden.cpp
    )
    target_include_directories(test_golden PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME golden_state COMMAND test_golden)
  else()
    message(STATUS "Skipping golden_state test: tests/test_golden.cpp not present")
  endif()

  if (EXISTS ${CMAKE_SOURCE_DIR}/tests/test_book_snapshot.cpp)
    add_executable(test_book_snapshot
      tests/test_book_snapshot.cpp
    )
    target_include_directories(test_book_snapshot PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(test_book_snapshot PRIVATE nlohmann_json::nlohmann_json)
    add_test(NAME book_snapshot COMMAND test_book_snapshot)
    set_tests_properties(book_snapshot PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  else()
    message(STATUS "Skipping book_snapshot test: tests/test_book_snapshot.cpp not present")
  endif()

  add_test(NAME replay_help COMMAND $<TARGET_FILE:replay> --help)
  set_tests_properties(replay_help PROPERTIES PASS_REGULAR_EXPRESSION "Blanc LOB Engine")
  add_test(NAME replay_default_run COMMAND $<TARGET_FILE:replay>)
  set_tests_properties(replay_default_run PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
