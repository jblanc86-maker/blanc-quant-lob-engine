// SPDX-License-Identifier: Apache-2.0
#include "breaker.hpp"
#include "detectors.hpp"
#include "telemetry.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace lob;

static uint64_t fnv1a(const std::vector<uint8_t> &v) {
  uint64_t h = 1469598103934665603ull;
  for (uint8_t b : v) {
    h ^= b;
    h *= 1099511628211ull;
  }
  return h;
}
static std::string hex64(uint64_t v) {
  char s[17];
  std::snprintf(s, sizeof(s), "%016llx", (unsigned long long)v);
  return s;
}
static size_t get_max_bytes() {
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

static bool read_all(const std::string &p, std::vector<uint8_t> &out) {
  std::ifstream f(p, std::ios::binary);
  if (!f)
    return false;
  f.seekg(0, std::ios::end);
  auto n = f.tellg();
  f.seekg(0, std::ios::beg);

  const std::streamsize max_size =
      static_cast<std::streamsize>(get_max_bytes());
  if (n < 0 || n > max_size) {
    std::cerr << "File size invalid or too large (max " << max_size
              << " bytes)\n";
    return false;
  }

  out.resize((size_t)n);
  return f.read(reinterpret_cast<char *>(out.data()), n).good();
}

static void print_help(const char *prog) {
  std::cout << "Blanc LOB Engine - Market Data Replay and Analysis\n\n"
            << "Usage: " << prog << " [OPTIONS]\n\n"
            << "Options:\n"
            << "  --input PATH        Input binary file path (default: "
               "data/golden/itch_1m.bin)\n"
            << "  --gap-ppm VALUE     Gap rate in parts per million (default: "
               "0)\n"
            << "  --corrupt-ppm VALUE Corrupt rate in parts per million "
               "(default: 0)\n"
            << "  --skew-ppm VALUE    Skew rate in parts per million (default: "
               "0)\n"
            << "  --burst-ms VALUE    Burst duration in milliseconds (default: "
               "0)\n"
            << "  --help, -h          Show this help message\n\n"
            << "Exit Codes:\n"
            << "  0 - Success\n"
            << "  1 - Invalid argument\n"
            << "  2 - File read error\n";
}

int main(int argc, char **argv) {
  std::string input = "data/golden/itch_1m.bin";
  double g = 0, c = 0, sk = 0, burst_ms = 0;
  for (int i = 1; i < argc; i++) {
    if (!std::strcmp(argv[i], "--help") || !std::strcmp(argv[i], "-h")) {
      print_help(argv[0]);
      return 0;
    } else if (!std::strcmp(argv[i], "--input") && i + 1 < argc)
      input = argv[++i];
    else if (!std::strcmp(argv[i], "--gap-ppm") && i + 1 < argc) {
      try {
        g = std::stod(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid --gap-ppm value: " << argv[i] << "\n";
        return 1;
      }
    } else if (!std::strcmp(argv[i], "--corrupt-ppm") && i + 1 < argc) {
      try {
        c = std::stod(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid --corrupt-ppm value: " << argv[i] << "\n";
        return 1;
      }
    } else if (!std::strcmp(argv[i], "--skew-ppm") && i + 1 < argc) {
      try {
        sk = std::stod(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid --skew-ppm value: " << argv[i] << "\n";
        return 1;
      }
    } else if (!std::strcmp(argv[i], "--burst-ms") && i + 1 < argc) {
      try {
        burst_ms = std::stod(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid --burst-ms value: " << argv[i] << "\n";
        return 1;
      }
    }
  }
  std::vector<uint8_t> buf;
  if (!read_all(input, buf)) {
    std::cerr << "Blanc LOB Engine: could not read " << input << "\n";
    return 2;
  }

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

  std::cout << "digest_fnv=0x" << t.actual_digest_hex
            << " breaker=" << Breaker::to_string(st)
            << " publish=" << (t.publish_allowed ? "YES" : "GATED") << "\n";
  return 0;
}
