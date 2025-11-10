// SPDX-License-Identifier: BUSL-1.1
#include "breaker.hpp"
#include <algorithm>
namespace lob {
Breaker::Breaker(const BreakerThresholds& t) : thr_(t) {}
BreakerState Breaker::state() const { return st_; }
bool Breaker::publish_allowed() const {
  return st_ == BreakerState::Fuse || st_ == BreakerState::Local;
}
void Breaker::clear_latch() {
  latched_ = false;
  if (st_ != BreakerState::Kill)
    st_ = BreakerState::Fuse;
}

static BreakerState worst(const DetectorReadings& r, const BreakerThresholds& t) {
  BreakerState w = BreakerState::Fuse;
  auto bump      = [&](BreakerState s) {
    if (int(s) > int(w))
      w = s;
  };
  if (r.gap_rate >= t.gap_ppm_main)
    bump(BreakerState::Main);
  else if (r.gap_rate >= t.gap_ppm_feeder)
    bump(BreakerState::Feeder);
  else if (r.gap_rate >= t.gap_ppm_local)
    bump(BreakerState::Local);

  if (r.corrupt_rate >= t.corrupt_ppm_main)
    bump(BreakerState::Main);
  else if (r.corrupt_rate >= t.corrupt_ppm_feeder)
    bump(BreakerState::Feeder);
  else if (r.corrupt_rate >= t.corrupt_ppm_local)
    bump(BreakerState::Local);

  if (r.skew_ppm >= t.skew_ppm_main)
    bump(BreakerState::Main);
  else if (r.skew_ppm >= t.skew_ppm_feeder)
    bump(BreakerState::Feeder);
  else if (r.skew_ppm >= t.skew_ppm_local)
    bump(BreakerState::Local);

  if (r.burst_ms >= t.burst_ms_main)
    bump(BreakerState::Main);
  else if (r.burst_ms >= t.burst_ms_feeder)
    bump(BreakerState::Feeder);
  else if (r.burst_ms >= t.burst_ms_local)
    bump(BreakerState::Local);
  return w;
}

BreakerState Breaker::step(const DetectorReadings& r) {
  BreakerState d = worst(r, thr_);
  if (latched_) {
    st_ = std::max(st_, d);
    return st_;
  }
  st_ = d;
  if (st_ == BreakerState::Feeder || st_ == BreakerState::Main || st_ == BreakerState::Kill)
    latched_ = true;
  return st_;
}

std::string Breaker::to_string(BreakerState s) {
  switch (s) {
  case BreakerState::Fuse:
    return "Fuse";
  case BreakerState::Local:
    return "Local";
  case BreakerState::Feeder:
    return "Feeder";
  case BreakerState::Main:
    return "Main";
  case BreakerState::Kill:
    return "Kill";
  }
  return "Unknown";
}
} // namespace lob
