cmake_minimum_required(VERSION 3.20)

# BQS Enterprise is intentionally additive and off by default.
project(bqs_enterprise LANGUAGES CXX)

add_library(bqs_enterprise INTERFACE)

target_include_directories(bqs_enterprise
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(bqs_enterprise INTERFACE cxx_std_20)

enable_testing()
add_subdirectory(tests)
