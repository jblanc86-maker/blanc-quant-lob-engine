// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "breaker.hpp"
#include <string>
namespace lob
{
  struct TelemetrySnapshot
  {
    std::string input_path, golden_digest_hex, actual_digest_hex;
    bool determinism_pass{false};
    double p50_ms{0.0}, p95_ms{0.0}, p99_ms{0.0};
    int cpu_pin{-1};
    DetectorReadings readings{};
    BreakerState breaker{};
    bool publish_allowed{true};
  };
  bool ensure_dir(const std::string &path);
  bool write_jsonl(const std::string &path, const TelemetrySnapshot &t);
  bool write_prom(const std::string &path, const TelemetrySnapshot &t);
  std::string now_iso8601();
} // namespace lob
