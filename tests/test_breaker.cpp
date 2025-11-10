#include "breaker.hpp"
#include <cassert>
#include <iostream>

using namespace lob;

int main() {
  BreakerThresholds thr{}; // defaults
  Breaker b(thr);
  // Initially Fuse
  assert(b.state() == BreakerState::Fuse);
  DetectorReadings r{};
  // No readings -> Fuse
  b.step(r);
  assert(b.state() == BreakerState::Fuse);
  // Trigger Local via gap
  r.gap_rate = thr.gap_ppm_local + 1;
  b.step(r);
  assert(b.state() == BreakerState::Local);
  // Trigger Feeder
  r.gap_rate = thr.gap_ppm_feeder + 1;
  b.step(r);
  assert(b.state() == BreakerState::Feeder);
  // Latch should engage (publish gated beyond Local)
  assert(!b.publish_allowed());
  // Clear latch resets to Fuse
  b.clear_latch();
  assert(b.state() == BreakerState::Fuse);
  std::cout << "test_breaker OK" << std::endl;
  return 0;
}
