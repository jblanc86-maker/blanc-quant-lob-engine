// SPDX-License-Identifier: Apache-2.0
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

int main() {
  const char* BIN = "./bin/replay"; // run from build dir
  const std::string ART_DIR = "../tests/out";
  const std::string INPUT = "../data/golden/itch_1m.bin";
  const std::string GOLDEN = "../tests/golden/book_snapshot.golden.json";

  // Ensure the directory exists
  std::string mkdir = "mkdir -p " + ART_DIR;
  system(mkdir.c_str());

  // Run replay and write to ART_DIR
  std::string cmd = "ART_DIR=" + ART_DIR + " " + BIN + " --input " + INPUT + " 2>&1";
  int rc = std::system(cmd.c_str());
  if (rc != 0) {
    std::cerr << "replay failed: rc=" << rc << std::endl;
    return 2;
  }

  // Read the latest bench.jsonl
  std::string bench_path = ART_DIR + "/bench.jsonl";
  std::ifstream f(bench_path);
  if (!f) {
    std::cerr << "could not read bench file: " << bench_path << std::endl;
    return 3;
  }
  std::string line;
  std::string last_line;
  while (std::getline(f, line)) {
    last_line = line;
  }
  if (last_line.empty()) {
    std::cerr << "bench.jsonl is empty" << std::endl;
    return 4;
  }

  // Parse JSON summary with simple regex for required fields
  std::string actual;
  {
    std::smatch m;
  std::regex r_actual("\"actual\"\\s*:\\s*\"([0-9a-fA-F]+)\"");
    if (std::regex_search(last_line, m, r_actual)) actual = m[1];
  }
  bool publish = false;
  {
    std::smatch m;
  std::regex r_publish("\"publish\"\\s*:\\s*(true|false)");
    if (std::regex_search(last_line, m, r_publish)) publish = (m[1] == "true");
  }
  std::string breaker;
  {
    std::smatch m;
  std::regex r_breaker("\"breaker\"\\s*:\\s*\"([a-zA-Z0-9_]+)\"");
    if (std::regex_search(last_line, m, r_breaker)) breaker = m[1];
  }

  // Load golden
  std::ifstream g(GOLDEN);
  if (!g) {
    std::cerr << "cannot open golden file: " << GOLDEN << std::endl;
    return 6;
  }
  std::string golden_line;
  std::getline(g, golden_line);
  std::string golden_actual;
  bool golden_publish = false;
  std::string golden_breaker;
  {
    std::smatch mg;
  std::regex rg_actual("\"actual\"\\s*:\\s*\"([0-9a-fA-F]+)\"");
    if (std::regex_search(golden_line, mg, rg_actual)) golden_actual = mg[1];
  }
  {
    std::smatch mg;
  std::regex rg_publish("\"publish\"\\s*:\\s*(true|false)");
    if (std::regex_search(golden_line, mg, rg_publish)) golden_publish = (mg[1] == "true");
  }
  {
    std::smatch mg;
  std::regex rg_breaker("\"breaker\"\\s*:\\s*\"([a-zA-Z0-9_]+)\"");
    if (std::regex_search(golden_line, mg, rg_breaker)) golden_breaker = mg[1];
  }

  // Compare fields: actual, publish, breaker
  bool ok = true;
  if (!golden_actual.empty()) {
    if (actual != golden_actual) {
      std::cerr << "actual digest mismatch: got " << actual << " expected " << golden_actual << std::endl;
      ok = false;
    }
  }
  if (golden_publish) {
    if (publish != golden_publish) {
      std::cerr << "publish mismatch: got " << publish << " expected " << golden_publish << std::endl;
      ok = false;
    }
  }
  if (!golden_breaker.empty()) {
    if (breaker != golden_breaker) {
      std::cerr << "breaker mismatch: got " << breaker << " expected " << golden_breaker << std::endl;
      ok = false;
    }
  }

  if (!ok) {
    std::cerr << "snapshot mismatch, dumping bench.jsonl last line: " << last_line << std::endl;
    return 8;
  }

  std::cout << "snapshot matches golden" << std::endl;
  return 0;
}
