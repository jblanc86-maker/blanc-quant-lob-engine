#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>
#include <vector>

#include "bench_util.hpp"

namespace {

std::vector<std::uint8_t>& synthetic_payload() {
    static std::vector<std::uint8_t> payload = [] {
        std::string seed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::vector<std::uint8_t> data(seed.size() * 1000);
        for (std::size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<std::uint8_t>(seed[i % seed.size()]);
        }
        return data;
    }();
    return payload;
}

}  // namespace

static void BM_Parsing_Synthetic(benchmark::State& state) {
    auto& payload = synthetic_payload();

    for (auto _ : state) {
        std::size_t parsed = 0;
        for (std::size_t i = 0; i < payload.size(); i += 8) {
            std::uint64_t word = 0;
            for (std::size_t b = 0; b < 8 && (i + b) < payload.size(); ++b) {
                word <<= 8;
                word |= payload[i + b];
            }
            do_not_optimize_away(word);
            ++parsed;
        }
        state.counters["records"] = static_cast<double>(parsed);
    }

    state.SetItemsProcessed(state.iterations() * payload.size());
}

BENCHMARK(BM_Parsing_Synthetic)
    ->Unit(benchmark::kNanosecond)
    ->UseRealTime();
