// SPDX-License-Identifier: BUSL-1.1
#include "telemetry.hpp"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
namespace fs = std::filesystem;
namespace lob {
bool ensure_dir(const std::string &p) {
  std::error_code ec;
  if (fs::exists(p, ec))
    return true;
  return fs::create_directories(p, ec);
}
static void esc(std::ostream &o, const std::string &s) {
  for (char c : s) {
    if (c == '"' || c == '\\')
      o << '\\' << c;
    else if (c == '\n')
      o << "\\n";
    else
      o << c;
  }
}
bool write_jsonl(const std::string &path, const TelemetrySnapshot &t) {
  std::ofstream f(path, std::ios::app);
  if (!f)
    return false;
  f << '{' << "\"ts\":\"" << now_iso8601() << "\","
    << "\"input\":\"";
  esc(f, t.input_path);
  f << "\","
    << "\"golden\":\"" << t.golden_digest_hex << "\","
    << "\"actual\":\"" << t.actual_digest_hex << "\","
    << "\"determinism\":" << (t.determinism_pass ? "true" : "false") << ","
    << "\"p50_ms\":" << t.p50_ms << ",\"p95_ms\":" << t.p95_ms
    << ",\"p99_ms\":" << t.p99_ms << ","
    << "\"gap_ppm\":" << t.readings.gap_rate << ","
    << "\"corrupt_ppm\":" << t.readings.corrupt_rate << ","
    << "\"skew_ppm\":" << t.readings.skew_ppm << ","
    << "\"burst_ms\":" << t.readings.burst_ms << ","
    << "\"breaker\":\"" << Breaker::to_string(t.breaker) << "\","
    << "\"publish\":" << (t.publish_allowed ? "true" : "false") << "}\n";
  return true;
}
bool write_prom(const std::string &path, const TelemetrySnapshot &t) {
  std::ofstream f(path);
  if (!f)
    return false;
  f << "lob_p50_ms " << t.p50_ms << "\n"
    << "lob_p95_ms " << t.p95_ms << "\n"
    << "lob_p99_ms " << t.p99_ms << "\n"
    << "lob_gap_ppm " << t.readings.gap_rate << "\n"
    << "lob_corrupt_ppm " << t.readings.corrupt_rate << "\n"
    << "lob_skew_ppm " << t.readings.skew_ppm << "\n"
    << "lob_burst_ms " << t.readings.burst_ms << "\n"
    << "lob_publish_allowed " << (t.publish_allowed ? 1 : 0) << "\n";
  return true;
}
std::string now_iso8601() {
  std::time_t t = std::time(nullptr);
  std::tm tm{};
  localtime_r(&t, &tm);
  std::ostringstream o;
  o << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S%z");
  return o.str();
}
} // namespace lob
