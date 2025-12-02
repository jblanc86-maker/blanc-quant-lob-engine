#include <benchmark/benchmark.h>

#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#include "bench_util.hpp"

namespace
{

  std::vector<std::uint32_t> &synthetic_messages()
  {
    static std::vector<std::uint32_t> messages = []
    {
      std::vector<std::uint32_t> data(1'000'000);
      std::mt19937 rng(42);
      std::uniform_int_distribution<std::uint32_t> dist(0, 1'000);
      for (auto &v : data)
      {
        v = dist(rng);
      }
      return data;
    }();
    return messages;
  }

} // namespace

// Benchmark: simulate replay work by walking preloaded synthetic messages.
static void BM_Replay_Synthetic_NoIO(benchmark::State &state)
{
  auto &messages = synthetic_messages();

  for (auto _ : state)
  {
    std::uint64_t accumulator = 0;
    for (auto value : messages)
    {
      accumulator += value;
    }
    do_not_optimize_away(accumulator);
  }

  state.counters["msgs"] = static_cast<double>(messages.size());
  state.SetItemsProcessed(state.iterations() * messages.size());
}

BENCHMARK(BM_Replay_Synthetic_NoIO)
    ->Unit(benchmark::kNanosecond)
    ->UseRealTime();
