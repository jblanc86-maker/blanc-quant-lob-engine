cmake_minimum_required(VERSION 3.20)
project(blanc_lob_engine VERSION 0.9 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(replay
  src/detectors.cpp
  src/replay.cpp
  src/breaker.cpp
  src/telemetry.cpp
)
target_include_directories(replay PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(replay PRIVATE -O3 -march=native -Wall -Wextra -Wpedantic)
