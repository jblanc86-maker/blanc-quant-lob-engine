cmake_minimum_required(VERSION 3.20)

# Google Benchmark via FetchContent
include(FetchContent)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)

# Disable benchmark's tests
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(benchmark)

add_executable(blanc_bench
    bench_main.cpp
    bench_replay.cpp
    bench_parsing.cpp
    bench_gates.cpp
)

target_link_libraries(blanc_bench
    PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        blanc_quant_lob_engine        # <- your main library/target name
)

target_compile_features(blanc_bench PRIVATE cxx_std_20)
target_compile_options(blanc_bench PRIVATE -O3 -DNDEBUG)

# Put benchmark binary under build/bench
set_target_properties(blanc_bench PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bench"
)
