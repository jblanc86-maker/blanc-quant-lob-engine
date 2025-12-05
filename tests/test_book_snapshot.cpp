// SPDX-License-Identifier: Apache-2.0
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <system_error>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <map>
#include <cctype>

namespace
{
  struct SimpleJson
  {
    std::map<std::string, std::string> strings;
    std::map<std::string, double> numbers;
    std::map<std::string, bool> bools;
  };

  bool parse_simple_json(const std::string &text, SimpleJson &out)
  {
    size_t i = 0;
    auto skip_ws = [&](void) {
      while (i < text.size() && isspace(static_cast<unsigned char>(text[i])))
        ++i;
    };
    skip_ws();
    if (i >= text.size() || text[i] != '{')
      return false;
    ++i;
    while (i < text.size())
    {
      skip_ws();
      if (i < text.size() && text[i] == '}')
      {
        ++i;
        break;
      }
      if (i >= text.size() || text[i] != '"')
        return false;
      size_t key_start = ++i;
      size_t key_end = text.find('"', key_start);
      if (key_end == std::string::npos)
        return false;
      std::string key = text.substr(key_start, key_end - key_start);
      i = key_end + 1;
      skip_ws();
      if (i >= text.size() || text[i] != ':')
        return false;
      ++i;
      skip_ws();
      if (i >= text.size())
        return false;
      if (text[i] == '"')
      {
        size_t val_start = ++i;
        size_t val_end = text.find('"', val_start);
        if (val_end == std::string::npos)
          return false;
        out.strings[key] = text.substr(val_start, val_end - val_start);
        i = val_end + 1;
      }
      else if (text.compare(i, 4, "true") == 0)
      {
        out.bools[key] = true;
        i += 4;
      }
      else if (text.compare(i, 5, "false") == 0)
      {
        out.bools[key] = false;
        i += 5;
      }
      else
      {
        size_t val_end = text.find_first_of(",}", i);
        if (val_end == std::string::npos)
          val_end = text.size();
        std::string num = text.substr(i, val_end - i);
        char *end_ptr = nullptr;
        double v = std::strtod(num.c_str(), &end_ptr);
        if (end_ptr == num.c_str())
          return false;
        out.numbers[key] = v;
        i = val_end;
      }
      skip_ws();
      if (i < text.size() && text[i] == ',')
      {
        ++i;
        continue;
      }
      else if (i < text.size() && text[i] == '}')
      {
        ++i;
        break;
      }
    }
    return true;
  }

  std::string read_text(const std::string &path)
  {
    std::ifstream f(path);
    if (!f)
      return {};
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
  }
}

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

  std::error_code ec;
  std::filesystem::create_directories(ART_DIR, ec);
  if (ec)
  {
    std::cerr << "failed to create output directory: " << ec.message() << std::endl;
    return 1;
  }

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

  SimpleJson bench_json;
  if (!parse_simple_json(last_line, bench_json))
  {
    std::cerr << "JSON parse error for bench.jsonl last line" << std::endl;
    return 5;
  }

  std::string actual = bench_json.strings["actual"];
  bool publish = bench_json.bools["publish"];
  std::string breaker = bench_json.strings["breaker"];

  std::string golden_text = read_text(GOLDEN);
  if (golden_text.empty())
  {
    std::cerr << "cannot open golden file: " << GOLDEN << std::endl;
    return 6;
  }
  SimpleJson golden_json;
  if (!parse_simple_json(golden_text, golden_json))
  {
    std::cerr << "JSON parse error for golden file" << std::endl;
    return 7;
  }

  std::string golden_actual = golden_json.strings["actual"];
  bool golden_publish = golden_json.bools["publish"];
  std::string golden_breaker = golden_json.strings["breaker"];
  std::string golden_input = golden_json.strings["input"];
  double golden_gap = golden_json.numbers.count("gap_ppm") ? golden_json.numbers["gap_ppm"] : -1.0;
  double golden_corrupt = golden_json.numbers.count("corrupt_ppm") ? golden_json.numbers["corrupt_ppm"] : -1.0;
  double golden_skew = golden_json.numbers.count("skew_ppm") ? golden_json.numbers["skew_ppm"] : -1.0;
  double golden_burst_ms = golden_json.numbers.count("burst_ms") ? golden_json.numbers["burst_ms"] : -1.0;
  int golden_cpu_pin = golden_json.numbers.count("cpu_pin") ? static_cast<int>(golden_json.numbers["cpu_pin"]) : -999;

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
  if (golden_json.bools.count("publish"))
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
    double gap_v = bench_json.numbers.count("gap_ppm") ? bench_json.numbers["gap_ppm"] : -1.0;
    if (std::abs(gap_v - golden_gap) > 1e-6)
    {
      std::cerr << "gap mismatch: got " << gap_v << " expected " << golden_gap << std::endl;
      ok = false;
    }
  }
  if (golden_corrupt >= 0)
  {
    double c_v = bench_json.numbers.count("corrupt_ppm") ? bench_json.numbers["corrupt_ppm"] : -1.0;
    if (std::abs(c_v - golden_corrupt) > 1e-6)
    {
      std::cerr << "corrupt mismatch: got " << c_v << " expected " << golden_corrupt << std::endl;
      ok = false;
    }
  }
  if (golden_skew >= 0)
  {
    double s_v = bench_json.numbers.count("skew_ppm") ? bench_json.numbers["skew_ppm"] : -1.0;
    if (std::abs(s_v - golden_skew) > 1e-6)
    {
      std::cerr << "skew mismatch: got " << s_v << " expected " << golden_skew << std::endl;
      ok = false;
    }
  }
  if (golden_burst_ms >= 0)
  {
    double b_v = bench_json.numbers.count("burst_ms") ? bench_json.numbers["burst_ms"] : -1.0;
    if (std::abs(b_v - golden_burst_ms) > 1e-6)
    {
      std::cerr << "burst mismatch: got " << b_v << " expected " << golden_burst_ms << std::endl;
      ok = false;
    }
  }
  if (golden_cpu_pin != -999)
  {
    int cp = bench_json.numbers.count("cpu_pin") ? static_cast<int>(bench_json.numbers["cpu_pin"]) : -999;
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
