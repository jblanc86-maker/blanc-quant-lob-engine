#include <benchmark/benchmark.h>

#include <cstddef>

#include "bench_util.hpp"

namespace
{

  class ExecutionGate
  {
  public:
    bool should_fire(std::size_t i)
    {
      return (i % 37) == 0;
    }
  };

} // namespace

static void BM_ExecutionGate_Overhead(benchmark::State &state)
{
  ExecutionGate gate;
  const std::size_t N = static_cast<std::size_t>(state.range(0));

  for (auto _ : state)
  {
    std::size_t trips = 0;
    for (std::size_t i = 0; i < N; ++i)
    {
      if (gate.should_fire(i))
      {
        ++trips;
      }
    }
    do_not_optimize_away(trips);
  }

  state.SetItemsProcessed(state.iterations() * N);
}

BENCHMARK(BM_ExecutionGate_Overhead)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Unit(benchmark::kNanosecond)
    ->UseRealTime();
