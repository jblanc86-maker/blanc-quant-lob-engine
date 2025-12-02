#include <chrono>
#include <cstdint>
#include <iostream>

int main()
{
  constexpr std::size_t N = 1'000'000;

  auto start = std::chrono::high_resolution_clock::now();

  std::uint64_t acc = 0;
  for (std::size_t i = 0; i < N; ++i)
  {
    acc += i; // stand-in for "do some work"
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  double per_op_ns = static_cast<double>(ns) / N;
  double per_op_us = per_op_ns / 1'000.0;

  std::cout << "Total ns: " << ns << "\n";
  std::cout << "Per op: " << per_op_us << " us\n";
  std::cout << "Acc: " << acc << "\n";
}
