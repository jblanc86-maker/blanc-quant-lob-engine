// SPDX-License-Identifier: Apache-2.0
#include "breaker.hpp"
#include "detectors.hpp"
#include "telemetry.hpp"
#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
using namespace lob;
namespace po = boost::program_options;

static uint64_t fnv1a(const std::vector<uint8_t> &v)
{
  uint64_t h = 1469598103934665603ull;
  for (uint8_t b : v)
  {
    h ^= b;
    h *= 1099511628211ull;
  }
  return h;
}
static std::string hex64(uint64_t v)
{
  char s[17];
  std::snprintf(s, sizeof(s), "%016llx", (unsigned long long)v);
  return s;
}
static size_t get_max_bytes()
{
  const char *env = std::getenv("REPLAY_MAX_BYTES");
  const size_t def = 128ull * 1024ull * 1024ull; // 128 MiB default
  if (!env || !*env)
    return def;
  char *end = nullptr;
  unsigned long long v = std::strtoull(env, &end, 10);
  if (end == env || v == 0)
    return def;
  return static_cast<size_t>(v);
}

static bool read_all(const std::string &p, std::vector<uint8_t> &out)
{
  std::ifstream f(p, std::ios::binary);
  if (!f)
    return false;
  f.seekg(0, std::ios::end);
  auto n = f.tellg();
  f.seekg(0, std::ios::beg);

  const std::streamsize max_size =
      static_cast<std::streamsize>(get_max_bytes());
  if (n < 0 || n > max_size)
  {
    std::cerr << "File size invalid or too large (max " << max_size
              << " bytes)\n";
    return false;
  }

  out.resize((size_t)n);
  return f.read(reinterpret_cast<char *>(out.data()), n).good();
}

int main(int argc, char **argv)
{
  po::options_description desc("Blanc LOB Engine - Market Data Replay and Analysis");
  desc.add_options()("help,h", "Show this help message")("input", po::value<std::string>()->default_value("data/golden/itch_1m.bin"),
                                                         "Input binary file path")("gap-ppm", po::value<double>()->default_value(0.0),
                                                                                   "Gap rate in parts per million")("corrupt-ppm", po::value<double>()->default_value(0.0),
                                                                                                                    "Corrupt rate in parts per million")("skew-ppm", po::value<double>()->default_value(0.0),
                                                                                                                                                         "Skew rate in parts per million")("burst-ms", po::value<double>()->default_value(0.0),
                                                                                                                                                                                           "Burst duration in milliseconds");

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch (const po::error &e)
  {
    std::cerr << "Error: " << e.what() << "\n\n";
    std::cerr << desc << "\n";
    std::cerr << "Exit Codes:\n"
              << "  0 - Success\n"
              << "  1 - Invalid argument\n"
              << "  2 - File read error\n";
    return 1;
  }

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    std::cout << "Exit Codes:\n"
              << "  0 - Success\n"
              << "  1 - Invalid argument\n"
              << "  2 - File read error\n";
    return 0;
  }

  std::string input = vm["input"].as<std::string>();
  double g = vm["gap-ppm"].as<double>();
  double c = vm["corrupt-ppm"].as<double>();
  double sk = vm["skew-ppm"].as<double>();
  double burst_ms = vm["burst-ms"].as<double>();
  std::vector<uint8_t> buf;
  if (!read_all(input, buf))
  {
    std::cerr << "Blanc LOB Engine: could not read " << input << "\n";
    return 2;
  }

  using clock = std::chrono::steady_clock;
  auto start = clock::now();
  uint64_t d = fnv1a(buf);
  Detectors det;
  det.on_message(buf.size());
  det.inject_ppm(g, c, sk, burst_ms);
  Breaker br(BreakerThresholds{});
  auto st = br.step(det.readings());

  ensure_dir("artifacts");
  TelemetrySnapshot t;
  t.input_path = input;
  t.golden_digest_hex = "<sha256-file>";
  t.actual_digest_hex = hex64(d);
  t.readings = det.readings();
  t.breaker = st;
  t.publish_allowed = br.publish_allowed();
  write_jsonl("artifacts/bench.jsonl", t);
  write_prom("artifacts/metrics.prom", t);

  auto end = clock::now();
  double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
  std::cout << "digest_fnv=0x" << std::hex << d
            << " breaker=" << Breaker::to_string(st)
            << " publish=" << (br.publish_allowed() ? "YES" : "NO")
            << " elapsed_ms=" << std::dec << elapsed_ms
            << std::endl;
  return 0;
}
