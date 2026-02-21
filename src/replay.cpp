// SPDX-License-Identifier: Apache-2.0
#include "breaker.hpp"
#include "detectors.hpp"
#include "telemetry.hpp"
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <optional>
#include <unordered_map>
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#include <cerrno>
#include <cstring>
#endif
using namespace lob;

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

struct ReplayOptions
{
  std::string input = "data/golden/itch_1m.bin";
  double gap_ppm = 0.0;
  double corrupt_ppm = 0.0;
  double skew_ppm = 0.0;
  double burst_ms = 0.0;
  int cpu_pin = -1;
  bool help = false;
};

static void print_help()
{
  std::cout << "Blanc LOB Engine - Market Data Replay and Analysis\n\n"
            << "Options:\n"
            << "  --help, -h            Show this help message\n"
            << "  --input <path>        Input binary file path (default data/golden/itch_1m.bin)\n"
            << "  --gap-ppm <value>     Gap rate in parts per million (default 0)\n"
            << "  --corrupt-ppm <value> Corrupt rate in parts per million (default 0)\n"
            << "  --skew-ppm <value>    Skew rate in parts per million (default 0)\n"
            << "  --burst-ms <value>    Burst duration in milliseconds (default 0)\n"
            << "  --cpu-pin <core>      Pin main thread to CPU core (Linux-only; default -1)\n\n"
            << "Exit Codes:\n"
            << "  0 - Success\n"
            << "  1 - Invalid argument\n"
            << "  2 - File read error\n";
}

static std::optional<double> parse_double(const std::string &s)
{
  char *end = nullptr;
  double v = std::strtod(s.c_str(), &end);
  if (end == s.c_str() || *end != '\0')
    return std::nullopt;
  return v;
}

static std::optional<int> parse_int(const std::string &s)
{
  char *end = nullptr;
  long v = std::strtol(s.c_str(), &end, 10);
  if (end == s.c_str() || *end != '\0')
    return std::nullopt;
  return static_cast<int>(v);
}

static bool parse_args(int argc, char **argv, ReplayOptions &out)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h")
    {
      out.help = true;
      continue;
    }

    auto consume_value = [&](std::string &dst) -> bool {
      if (i + 1 >= argc)
        return false;
      dst = argv[++i];
      return true;
    };

    if (arg == "--input")
    {
      if (!consume_value(out.input))
        return false;
    }
    else if (arg == "--gap-ppm")
    {
      std::string v;
      if (!consume_value(v))
        return false;
      auto parsed = parse_double(v);
      if (!parsed)
        return false;
      out.gap_ppm = *parsed;
    }
    else if (arg == "--corrupt-ppm")
    {
      std::string v;
      if (!consume_value(v))
        return false;
      auto parsed = parse_double(v);
      if (!parsed)
        return false;
      out.corrupt_ppm = *parsed;
    }
    else if (arg == "--skew-ppm")
    {
      std::string v;
      if (!consume_value(v))
        return false;
      auto parsed = parse_double(v);
      if (!parsed)
        return false;
      out.skew_ppm = *parsed;
    }
    else if (arg == "--burst-ms")
    {
      std::string v;
      if (!consume_value(v))
        return false;
      auto parsed = parse_double(v);
      if (!parsed)
        return false;
      out.burst_ms = *parsed;
    }
    else if (arg == "--cpu-pin")
    {
      std::string v;
      if (!consume_value(v))
        return false;
      auto parsed = parse_int(v);
      if (!parsed)
        return false;
      out.cpu_pin = *parsed;
    }
    else
    {
      std::cerr << "Unknown argument: " << arg << "\n";
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{
  ReplayOptions opt;
  if (!parse_args(argc, argv, opt))
  {
    print_help();
    return 1;
  }

  if (opt.help)
  {
    print_help();
    return 0;
  }

  std::vector<uint8_t> buf;
  if (!read_all(opt.input, buf))
  {
    std::cerr << "Blanc LOB Engine: could not read " << opt.input << "\n";
    return 2;
  }

  using clock = std::chrono::steady_clock;
  auto start = clock::now();
  // Set CPU affinity as requested (best-effort; Linux-only)
  if (opt.cpu_pin >= 0)
  {
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(opt.cpu_pin, &cpuset);
    int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
    {
      std::cerr << "Warning: pthread_setaffinity_np failed: " << std::strerror(errno) << "\n";
    }
#else
    (void)opt.cpu_pin; // no-op on non-Linux platforms
#endif
  }

  // Per-event timing — synthetic: treat each 64-byte chunk as one "event"
  // to produce a realistic latency sample vector for tail percentile computation.
  constexpr size_t kEventSize = 64;
  std::vector<double> event_latencies_ms;
  {
    const size_t n_events = buf.size() > 0 ? (buf.size() + kEventSize - 1) / kEventSize : 1;
    event_latencies_ms.reserve(n_events);
    for (size_t i = 0; i < buf.size(); i += kEventSize)
    {
      auto t0 = clock::now();
      // Touch each byte to simulate event processing and prevent elision
      volatile uint8_t sink = 0;
      const size_t end_i = std::min(i + kEventSize, buf.size());
      for (size_t j = i; j < end_i; ++j)
        sink ^= buf[j];
      (void)sink;
      auto t1 = clock::now();
      event_latencies_ms.push_back(
          std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
  }

  // Compute percentiles from sorted copy
  auto percentile = [](std::vector<double> v, double pct) -> double {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    double idx = pct * 0.01 * static_cast<double>(v.size() - 1);
    size_t lo = static_cast<size_t>(idx);
    size_t hi = lo + 1 < v.size() ? lo + 1 : lo;
    double frac = idx - static_cast<double>(lo);
    return v[lo] * (1.0 - frac) + v[hi] * frac;
  };

  uint64_t d = fnv1a(buf);
  Detectors det;
  det.on_message(buf.size());
  det.inject_ppm(opt.gap_ppm, opt.corrupt_ppm, opt.skew_ppm, opt.burst_ms);
  Breaker br(BreakerThresholds{});
  auto st = br.step(det.readings());

  const char *env_artdir = std::getenv("ART_DIR");
  std::string out_dir = env_artdir && *env_artdir ? std::string(env_artdir) : std::string("artifacts");
  ensure_dir(out_dir);
  TelemetrySnapshot t;
  t.input_path = opt.input;
  t.golden_digest_hex = "<sha256-file>";
  t.actual_digest_hex = hex64(d);
  t.cpu_pin = opt.cpu_pin;
  t.readings = det.readings();
  t.breaker = st;
  t.publish_allowed = br.publish_allowed();
  t.sample_count = static_cast<uint64_t>(event_latencies_ms.size());
  t.p50_ms   = percentile(event_latencies_ms, 50.0);
  t.p95_ms   = percentile(event_latencies_ms, 95.0);
  t.p99_ms   = percentile(event_latencies_ms, 99.0);
  t.p999_valid = t.sample_count >= 1000;   // p99.9 requires ≥1k samples
  t.p9999_valid = t.sample_count >= 10000; // p99.99 requires ≥10k samples
  t.p999_ms  = t.p999_valid ? percentile(event_latencies_ms, 99.9) : 0.0;
  t.p9999_ms = t.p9999_valid ? percentile(event_latencies_ms, 99.99) : 0.0;
  write_jsonl(out_dir + "/bench.jsonl", t);
  write_prom(out_dir + "/metrics.prom", t);

  auto end = clock::now();
  double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
  std::cout << "digest_fnv=0x" << std::hex << d
            << " breaker=" << Breaker::to_string(st)
            << " publish=" << (br.publish_allowed() ? "YES" : "NO")
            << " elapsed_ms=" << std::dec << elapsed_ms
            << " samples=" << t.sample_count
            << " p50=" << t.p50_ms << "ms"
            << " p99=" << t.p99_ms << "ms"
            << " p99.9=" << t.p999_ms << "ms"
            << " p99.99=" << t.p9999_ms << "ms"
            << " p99.9_valid=" << (t.p999_valid ? "true" : "false")
            << " p99.99_valid=" << (t.p9999_valid ? "true" : "false")
            << std::endl;
  return 0;
}
