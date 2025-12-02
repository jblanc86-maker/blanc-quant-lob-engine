#pragma once

#include <benchmark/benchmark.h>
#include <chrono>
#include <cstdint>
#include <cstdio>

#if defined(__GNUC__) || defined(__clang__)
#  define BQ_NOINLINE __attribute__((noinline))
#else
#  define BQ_NOINLINE
#endif

// Prevents compiler from optimizing away work in microbenchmarks
template <typename T>
BQ_NOINLINE void do_not_optimize_away(const T& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}

// Simple RAII timer if you ever want non-benchmark measurements.
class ScopedTimer {
public:
    using clock = std::chrono::high_resolution_clock;

    explicit ScopedTimer(const char* label)
        : label_(label), start_(clock::now()) {}

    ~ScopedTimer() {
        auto end   = clock::now();
        auto ns    = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
        double ms  = static_cast<double>(ns) / 1'000'000.0;
        std::fprintf(stderr, "[%s] elapsed: %.3f ms\n", label_, ms);
    }

private:
    const char* label_;
    clock::time_point start_;
};
