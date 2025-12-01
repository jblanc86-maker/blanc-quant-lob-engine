// SPDX-License-Identifier: Apache-2.0
#include <filesystem>
#include <iostream>
#include <system_error>
#include <fstream>
#include <string>
#include "telemetry.hpp"

using namespace lob;

int main()
{
  TelemetrySnapshot t;
  t.input_path = "data/golden/itch_1m.bin";
  t.golden_digest_hex = "deadbeef";
  t.actual_digest_hex = "cafef00d";
  t.determinism_pass = true;
  t.p50_ms = 1.1;
  t.p95_ms = 2.2;
  t.p99_ms = 3.3;
  t.cpu_pin = 0;
  t.readings.gap_rate = 0.1;
  t.breaker = BreakerState::Fuse;
  t.publish_allowed = true;

  const std::filesystem::path out_dir{"tests/out"};
  const std::string json_file = (out_dir / "telemetry.jsonl").string();
  const std::string prom_file = (out_dir / "metrics.prom").string();

  std::error_code ec;
  std::filesystem::create_directories(out_dir, ec);
  if (ec)
  {
    std::cerr << "failed to create output dir: " << ec.message() << std::endl;
    return 10;
  }

  if (!write_jsonl(json_file, t))
  {
    std::cerr << "write_jsonl failed" << std::endl;
    return 1;
  }
  if (!write_prom(prom_file, t))
  {
    std::cerr << "write_prom failed" << std::endl;
    return 2;
  }

  // Validate the files contain expected content
  std::ifstream fjson(json_file);
  if (!fjson)
  {
    std::cerr << "json file missing" << std::endl;
    return 3;
  }
  std::string contents((std::istreambuf_iterator<char>(fjson)), std::istreambuf_iterator<char>());
  if (contents.find("\"actual\":\"cafef00d\"") == std::string::npos)
  {
    std::cerr << "actual digest not found in json" << std::endl;
    return 4;
  }

  std::ifstream fprom(prom_file);
  if (!fprom)
  {
    std::cerr << "prom file missing" << std::endl;
    return 5;
  }
  std::string prom_contents((std::istreambuf_iterator<char>(fprom)), std::istreambuf_iterator<char>());
  if (prom_contents.find("lob_cpu_pin 0") == std::string::npos)
  {
    std::cerr << "lob_cpu_pin not found in prom file" << std::endl;
    return 6;
  }

  std::cout << "telemetry io test passed" << std::endl;
  return 0;
}
