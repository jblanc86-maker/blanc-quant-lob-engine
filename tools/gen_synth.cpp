// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
int main(int argc, char **argv) {
  size_t count = 1'000'000; // records
  std::string out = "data/golden/itch_1m.bin";
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    if (a == "--count" && i + 1 < argc)
      count = std::stoull(argv[++i]);
    else if (a == "--out" && i + 1 < argc)
      out = argv[++i];
  }
  std::mt19937_64 rng(0xB1A9C0FFEEULL); // fixed seed for determinism
  std::uniform_int_distribution<uint64_t> d;
  std::ofstream f(out, std::ios::binary);
  std::vector<uint64_t> buf(4096);
  size_t left = count;
  while (left) {
    size_t n = std::min(left, buf.size());
    for (size_t i = 0; i < n; i++)
      buf[i] = d(rng);
    f.write(reinterpret_cast<char *>(buf.data()), n * sizeof(uint64_t));
    left -= n;
  }
  std::cerr << "wrote " << out << "\n";
}
