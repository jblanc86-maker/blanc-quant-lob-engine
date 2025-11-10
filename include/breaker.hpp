// SPDX-License-Identifier: BSL-1.1
#pragma once
#include <cstdint>
#include <string>

namespace lob {
enum class BreakerState : uint8_t {
  Fuse = 0,
  Local = 1,
  Feeder = 2,
  Main = 3,
  Kill = 4
};

struct DetectorReadings {
  double gap_rate{0.0};
  double corrupt_rate{0.0};
  double skew_ppm{0.0};
  double burst_ms{0.0};
};

struct BreakerThresholds {
  double gap_ppm_local{5}, gap_ppm_feeder{50}, gap_ppm_main{200};
  double corrupt_ppm_local{1}, corrupt_ppm_feeder{10}, corrupt_ppm_main{50};
  double skew_ppm_local{5}, skew_ppm_feeder{20}, skew_ppm_main{100};
  double burst_ms_local{2.0}, burst_ms_feeder{5.0}, burst_ms_main{10.0};
};

class Breaker {
public:
  explicit Breaker(const BreakerThresholds &t);
  BreakerState state() const;
  bool publish_allowed() const;
  void clear_latch();
  BreakerState step(const DetectorReadings &r);
  static std::string to_string(BreakerState s);

private:
  BreakerThresholds thr_;
  BreakerState st_{BreakerState::Fuse};
  bool latched_{false};
};
} // namespace lob
