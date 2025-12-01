// SPDX-License-Identifier: Apache-2.0
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

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

  // Parse JSON summary with regex for required fields
  std::string actual;
  {
    std::smatch m;
    std::regex r_actual("\"actual\"\\s*:\\s*\"([0-9a-fA-F]+)\"");
    if (std::regex_search(last_line, m, r_actual))
      actual = m[1];
  }
  bool publish = false;
  {
    std::smatch m;
    std::regex r_publish("\"publish\"\\s*:\\s*(true|false)");
    if (std::regex_search(last_line, m, r_publish))
      publish = (m[1] == "true");
  }
  std::string breaker;
  {
    std::smatch m;
    std::regex r_breaker("\"breaker\"\\s*:\\s*\"([a-zA-Z0-9_]+)\"");
    if (std::regex_search(last_line, m, r_breaker))
      breaker = m[1];
  }

  // Load golden
  std::ifstream g(GOLDEN);
  if (!g)
  {
    std::cerr << "cannot open golden file: " << GOLDEN << std::endl;
    return 6;
  }
  std::string golden_line;
  std::getline(g, golden_line);
  std::string golden_actual;
  bool golden_publish = false;
  std::string golden_breaker;
  std::string golden_input;
  double golden_gap = -1;
  double golden_corrupt = -1;
  double golden_skew = -1;
  double golden_burst_ms = -1;
  int golden_cpu_pin = -999;
  {
    std::smatch mg;
    std::regex rg_actual("\"actual\"\\s*:\\s*\"([0-9a-fA-F]+)\"");
    if (std::regex_search(golden_line, mg, rg_actual))
      golden_actual = mg[1];
  }
  {
    std::smatch mg;
    std::regex rg_input("\"input\"\\s*:\\s*\"([^\"]+)\"");
    if (std::regex_search(golden_line, mg, rg_input))
      golden_input = mg[1];
  }
  {
    std::smatch mg;
    std::regex rg_gap("\"gap_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    if (std::regex_search(golden_line, mg, rg_gap))
      golden_gap = std::stod(mg[1]);
  }
  {
    std::smatch mg;
    std::regex rg_corrupt("\"corrupt_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    if (std::regex_search(golden_line, mg, rg_corrupt))
      golden_corrupt = std::stod(mg[1]);
  }
  {
    std::smatch mg;
    std::regex rg_skew("\"skew_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    if (std::regex_search(golden_line, mg, rg_skew))
      golden_skew = std::stod(mg[1]);
  }
  {
    std::smatch mg;
    std::regex rg_burst("\"burst_ms\"\\s*:\\s*([0-9\.eE+-]+)");
    if (std::regex_search(golden_line, mg, rg_burst))
      golden_burst_ms = std::stod(mg[1]);
  }
  {
    std::smatch mg;
    std::regex rg_cpu("\"cpu_pin\"\\s*:\\s*(-?[0-9]+)");
    if (std::regex_search(golden_line, mg, rg_cpu))
      golden_cpu_pin = std::stoi(mg[1]);
  }
  {
    std::smatch mg;
    std::regex rg_publish("\"publish\"\\s*:\\s*(true|false)");
    if (std::regex_search(golden_line, mg, rg_publish))
      golden_publish = (mg[1] == "true");
  }
  {
    std::smatch mg;
    std::regex rg_breaker("\"breaker\"\\s*:\\s*\"([a-zA-Z0-9_]+)\"");
    if (std::regex_search(golden_line, mg, rg_breaker))
      golden_breaker = mg[1];
  }

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
    std::smatch m;
    std::regex rg_gapv("\"gap_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    double gap_v = -1;
    if (std::regex_search(last_line, m, rg_gapv))
      gap_v = std::stod(m[1]);
    if (std::abs(gap_v - golden_gap) > 1e-6)
    {
      std::cerr << "gap mismatch: got " << gap_v << " expected " << golden_gap << std::endl;
      ok = false;
    }
  }
  if (golden_corrupt >= 0)
  {
    std::smatch m;
    std::regex rg_cv("\"corrupt_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    double c_v = -1;
    if (std::regex_search(last_line, m, rg_cv))
      c_v = std::stod(m[1]);
    if (std::abs(c_v - golden_corrupt) > 1e-6)
    {
      std::cerr << "corrupt mismatch: got " << c_v << " expected " << golden_corrupt << std::endl;
      ok = false;
    }
  }
  if (golden_skew >= 0)
  {
    std::smatch m;
    std::regex rg_sv("\"skew_ppm\"\\s*:\\s*([0-9\.eE+-]+)");
    double s_v = -1;
    if (std::regex_search(last_line, m, rg_sv))
      s_v = std::stod(m[1]);
    if (std::abs(s_v - golden_skew) > 1e-6)
    {
      std::cerr << "skew mismatch: got " << s_v << " expected " << golden_skew << std::endl;
      ok = false;
    }
  }
  if (golden_burst_ms >= 0)
  {
    std::smatch m;
    std::regex rg_bv("\"burst_ms\"\\s*:\\s*([0-9\.eE+-]+)");
    double b_v = -1;
    if (std::regex_search(last_line, m, rg_bv))
      b_v = std::stod(m[1]);
    if (std::abs(b_v - golden_burst_ms) > 1e-6)
    {
      std::cerr << "burst mismatch: got " << b_v << " expected " << golden_burst_ms << std::endl;
      ok = false;
    }
  }
  if (golden_cpu_pin != -999)
  {
    std::smatch m;
    std::regex rg_cp("\"cpu_pin\"\\s*:\\s*(-?[0-9]+)");
    int cp = -999;
    if (std::regex_search(last_line, m, rg_cp))
      cp = std::stoi(m[1]);
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
