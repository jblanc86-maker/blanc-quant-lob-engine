#include "telemetry.hpp"
#include <cassert>
#include <filesystem>
#include <iostream>

using namespace lob;
namespace fs = std::filesystem;

int main() {
  // Ensure temp artifacts dir exists
  const std::string dir = "artifacts";
  assert(ensure_dir(dir));

  TelemetrySnapshot t{};
  t.input_path = "data/golden/itch_1m.bin";
  t.golden_digest_hex = "deadbeef";
  t.actual_digest_hex = "cafebabe";
  t.determinism_pass = true;
  t.p50_ms = 1.23; t.p95_ms = 2.34; t.p99_ms = 3.45;

  const std::string jsonl = dir + "/test_ci.jsonl";
  const std::string prom  = dir + "/test_ci.metrics.prom";

  bool ok_jsonl = write_jsonl(jsonl, t);
  bool ok_prom  = write_prom(prom, t);
  assert(ok_jsonl);
  assert(ok_prom);
  assert(fs::exists(jsonl));
  assert(fs::exists(prom));
  std::cout << "test_telemetry OK" << std::endl;
  return 0;
}
