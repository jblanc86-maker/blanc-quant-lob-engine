// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "breaker.hpp"
#include <cstdint>
namespace lob {
class Detectors {
public:
  explicit Detectors(double a = 0.2) : a_(a) {}
  void on_message(uint64_t total) { total_ = total; }
  void on_gap(uint64_t inc = 1) { gaps_ += inc; }
  void on_corrupt(uint64_t inc = 1) { corrupt_ += inc; }
  void on_burst(double ms) {
    if (ms > burst_ms_)
      burst_ms_ = ms;
  }
  void on_skew_ppm(double ppm) { skew_ppm_ = ppm; }
  void inject_ppm(double gap, double corr, double skew, double burst) {
    s_gap_ = gap;
    s_corr_ = corr;
    s_skew_ = skew;
    s_burst_ = burst;
  }
  DetectorReadings readings() const {
    auto ppm = [&](uint64_t part) {
      return total_ ? (double(part) * 1'000'000.0 / double(total_)) : 0.0;
    };
    double rg = ppm(gaps_), rc = ppm(corrupt_), rb = burst_ms_, rs = skew_ppm_;
    return DetectorReadings{
        a_ * rg + (1 - a_) * s_gap_, a_ * rc + (1 - a_) * s_corr_,
        a_ * rs + (1 - a_) * s_skew_, a_ * rb + (1 - a_) * s_burst_};
  }

private:
  double a_;
  uint64_t total_{0}, gaps_{0}, corrupt_{0};
  double burst_ms_{0.0}, skew_ppm_{0.0};
  double s_gap_{0.0}, s_corr_{0.0}, s_skew_{0.0}, s_burst_{0.0};
};
} // namespace lob
