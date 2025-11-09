// SPDX-License-Identifier: BSL-1.1
#include "detectors.hpp"
#include <algorithm>

namespace lob {

Detectors::Detectors(double ewma_alpha) : a_(ewma_alpha) {}
void Detectors::on_message(uint64_t total_msgs) { total_ = total_msgs; }
void Detectors::on_gap(uint64_t inc) { gaps_ += inc; }
void Detectors::on_corrupt(uint64_t inc) { corrupt_ += inc; }
void Detectors::on_burst(double ms) { burst_ms_ = std::max(burst_ms_, ms); }
void Detectors::on_skew_ppm(double ppm) { skew_ppm_ = ppm; }

void Detectors::inject_ppm(double gap_ppm, double corrupt_ppm, double skew_ppm, double burst_ms) {
  s_gap_ppm_ = gap_ppm; s_corrupt_ppm_ = corrupt_ppm; s_skew_ppm_ = skew_ppm; s_burst_ms_ = burst_ms;
}

static inline double ppm(uint64_t part, uint64_t whole) {
  if (whole == 0) return 0.0;
  return (static_cast<double>(part) * 1'000'000.0) / static_cast<double>(whole);
}

DetectorReadings Detectors::readings() const {
  double raw_gap     = ppm(gaps_, total_);
  double raw_corrupt = ppm(corrupt_, total_);
  double raw_burst   = burst_ms_;
  double raw_skew    = skew_ppm_;
  DetectorReadings r;
  r.gap_rate     = a_*raw_gap     + (1-a_)*s_gap_ppm_;
  r.corrupt_rate = a_*raw_corrupt + (1-a_)*s_corrupt_ppm_;
  r.burst_ms     = a_*raw_burst   + (1-a_)*s_burst_ms_;
  r.skew_ppm     = a_*raw_skew    + (1-a_)*s_skew_ppm_;
  return r;
}

} // namespace lob
