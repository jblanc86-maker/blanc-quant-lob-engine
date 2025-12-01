// SPDX-License-Identifier: Apache-2.0
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <nlohmann/json.hpp>

int main(int argc, char **argv)
{
  const char *BIN = "./bin/replay"; // run from build dir
  const std::string ART_DIR = "../tests/out";
  const std::string INPUT = "../data/golden/itch_1m.bin";
  const std::string input = "data/golden/itch_1m.bin"; // value used in JSON
  std::string GOLDEN = "../tests/golden/book_snapshot.golden.json";
  bool burst_mode = false;
  if (argc > 1 && std::string(argv[1]) == "burst")
  {
    burst_mode = true;
    GOLDEN = "../tests/golden/book_snapshot_burst.golden.json";
  }

  // Ensure the directory exists
  std::string mkdir = "mkdir -p " + ART_DIR;
  system(mkdir.c_str());

  // Run replay and write to ART_DIR
  std::string cmd = "ART_DIR=" + ART_DIR + " " + BIN + " --input " + INPUT + " 2>&1";
  if (burst_mode)
  {
    cmd = "ART_DIR=" + ART_DIR + " " + BIN + " --input " + INPUT + " --burst-ms 100 2>&1";
  }
  int rc = std::system(cmd.c_str());
  if (rc != 0)
  {
    std::cerr << "replay failed: rc=" << rc << std::endl;
    return 2;
  }

  // Read the latest bench.jsonl
  std::string bench_path = ART_DIR + "/bench.jsonl";
  std::ifstream f(bench_path);
  if (!f)
  {
    std::cerr << "could not read bench file: " << bench_path << std::endl;
    return 3;
  }
  std::string line;
  std::string last_line;
  while (std::getline(f, line))
  {
    last_line = line;
  }
  if (last_line.empty())
  {
    std::cerr << "bench.jsonl is empty" << std::endl;
    return 4;
  }

  // Parse JSON summary for bench.jsonl
  nlohmann::json j;
  try
  {
    j = nlohmann::json::parse(last_line);
  }
  catch (const std::exception &e)
  {
    std::cerr << "JSON parse error for bench.jsonl last line: " << e.what() << std::endl;
    return 5;
  }

  std::string actual = j.value("actual", std::string());
  bool publish = j.value("publish", false);
  std::string breaker = j.value("breaker", std::string());

  // Load golden
  std::ifstream g(GOLDEN);
  if (!g)
  {
    std::cerr << "cannot open golden file: " << GOLDEN << std::endl;
    return 6;
  }
  nlohmann::json gj;
  try
  {
    g >> gj;
  }
  catch (const std::exception &e)
  {
    std::cerr << "JSON parse error for golden file: " << e.what() << std::endl;
    return 7;
  }

  std::string golden_actual = gj.value("actual", std::string());
  bool golden_publish = gj.value("publish", false);
  std::string golden_breaker = gj.value("breaker", std::string());
  std::string golden_input = gj.value("input", std::string());
  double golden_gap = gj.value("gap_ppm", -1.0);
  double golden_corrupt = gj.value("corrupt_ppm", -1.0);
  double golden_skew = gj.value("skew_ppm", -1.0);
  double golden_burst_ms = gj.value("burst_ms", -1.0);
  int golden_cpu_pin = gj.value("cpu_pin", -999);

  // Compare fields: input, actual, publish, breaker, detectors and cpu_pin
  bool ok = true;
  if (!golden_input.empty())
  {
    if (input != golden_input)
    {
      std::cerr << "input mismatch: got " << input << " expected " << golden_input << std::endl;
      ok = false;
    }
  }
  if (!golden_actual.empty())
  {
    if (actual != golden_actual)
    {
      std::cerr << "actual digest mismatch: got " << actual << " expected " << golden_actual << std::endl;
      ok = false;
    }
  }
  if (golden_publish)
  {
    if (publish != golden_publish)
    {
      std::cerr << "publish mismatch: got " << publish << " expected " << golden_publish << std::endl;
      ok = false;
    }
  }
  if (!golden_breaker.empty())
  {
    if (breaker != golden_breaker)
    {
      std::cerr << "breaker mismatch: got " << breaker << " expected " << golden_breaker << std::endl;
      ok = false;
    }
  }

  if (golden_gap >= 0)
  {
    double gap_v = j.value("gap_ppm", -1.0);
    if (std::abs(gap_v - golden_gap) > 1e-6)
    {
      std::cerr << "gap mismatch: got " << gap_v << " expected " << golden_gap << std::endl;
      ok = false;
    }
  }
  if (golden_corrupt >= 0)
  {
    double c_v = j.value("corrupt_ppm", -1.0);
    if (std::abs(c_v - golden_corrupt) > 1e-6)
    {
      std::cerr << "corrupt mismatch: got " << c_v << " expected " << golden_corrupt << std::endl;
      ok = false;
    }
  }
  if (golden_skew >= 0)
  {
    double s_v = j.value("skew_ppm", -1.0);
    if (std::abs(s_v - golden_skew) > 1e-6)
    {
      std::cerr << "skew mismatch: got " << s_v << " expected " << golden_skew << std::endl;
      ok = false;
    }
  }
  if (golden_burst_ms >= 0)
  {
    double b_v = j.value("burst_ms", -1.0);
    if (std::abs(b_v - golden_burst_ms) > 1e-6)
    {
      std::cerr << "burst mismatch: got " << b_v << " expected " << golden_burst_ms << std::endl;
      ok = false;
    }
  }
  if (golden_cpu_pin != -999)
  {
    int cp = static_cast<int>(j.value("cpu_pin", -999));
    if (cp != golden_cpu_pin)
    {
      std::cerr << "cpu_pin mismatch: got " << cp << " expected " << golden_cpu_pin << std::endl;
      ok = false;
    }
  }

  if (!ok)
  {
    std::cerr << "snapshot mismatch, dumping bench.jsonl last line: " << last_line << std::endl;
    return 8;
  }

  std::cout << "snapshot matches golden" << std::endl;
  return 0;
}
