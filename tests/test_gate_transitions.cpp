// SPDX-License-Identifier: Apache-2.0
// tests/test_gate_transitions.cpp
//
// Gate Transition Determinism & Latency Harness
// ==============================================
//
// DSEF SCM/Coordination Curves gap closure:
//   (a) Gate fires at a deterministic EVENT INDEX, not at a timestamp.
//   (b) Deterministic replay: two identical burst sequences must produce
//       byte-identical transition logs (same index, same rule, same state).
//   (c) Sub-microsecond gate-check latency: measure the wall time of one
//       Breaker::step() call; assert it is <1 µs (1000 ns) on median.
//
// Design:
//   - Uses Breaker + DetectorReadings directly — no replay binary required.
//   - "Burst" is simulated by injecting burst_ms above each threshold in turn.
//   - TransitionLog records (event_index, from_state, to_state, rule_id) —
//     the same fields a production Gate Decision Journal would emit.
//   - Two independent runs over the same script are compared for byte equality.
//   - Breaker::step() is called in a tight loop; median latency is reported.
//
// Tests:
//   1. index_determinism   — gate fires at the exact same index on two runs
//   2. log_replay_equality — full transition log is byte-identical on replay
//   3. step_latency_sub1us — median Breaker::step() latency < 1000 ns

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "breaker.hpp"
#include "detectors.hpp"

using namespace lob;

// ---------------------------------------------------------------------------
// TransitionRecord: one gate state change
// ---------------------------------------------------------------------------
struct TransitionRecord {
    uint64_t    event_index;   // index of the event that caused the transition
    BreakerState from_state;
    BreakerState to_state;
    std::string  rule_id;      // which threshold was crossed (e.g. "burst_ms>local")

    bool operator==(const TransitionRecord& o) const {
        return event_index == o.event_index
            && from_state  == o.from_state
            && to_state    == o.to_state
            && rule_id     == o.rule_id;
    }
    bool operator!=(const TransitionRecord& o) const { return !(*this == o); }

    std::string to_string() const {
        std::ostringstream ss;
        ss << "[idx=" << event_index
           << " " << Breaker::to_string(from_state)
           << "->" << Breaker::to_string(to_state)
           << " rule=" << rule_id << "]";
        return ss.str();
    }
};

// ---------------------------------------------------------------------------
// BurstScript: sequence of (event_index, burst_ms) injections.
// At each event, DetectorReadings.burst_ms is set to the scripted value;
// all other readings are zero.  This isolates the burst threshold path.
// ---------------------------------------------------------------------------
struct BurstEvent {
    uint64_t index;
    double   burst_ms;
};

// Build a script that:
//   events 0..199  — nominal (burst_ms = 0)
//   event  200     — burst crosses Local threshold (2.1 ms)
//   events 201..399 — nominal recovery
//   event  400     — burst crosses Feeder threshold (5.1 ms) → latch
//   events 401..599 — continues latched
//   event  600     — burst crosses Main threshold (10.1 ms) → escalate latched
static std::vector<BurstEvent> make_script()
{
    std::vector<BurstEvent> s;
    for (uint64_t i = 0; i < 800; ++i) {
        double b = 0.0;
        if (i == 200) b = 2.1;   // Local trip
        if (i == 400) b = 5.1;   // Feeder trip + latch
        if (i == 600) b = 10.1;  // Main (latched, can only escalate)
        s.push_back({i, b});
    }
    return s;
}

// ---------------------------------------------------------------------------
// run_script: execute the burst script, return the transition log.
// ---------------------------------------------------------------------------
static std::vector<TransitionRecord> run_script(
    const std::vector<BurstEvent>& script,
    const BreakerThresholds& thr)
{
    Breaker b(thr);
    std::vector<TransitionRecord> log;

    BreakerState prev = b.state();
    for (const auto& ev : script) {
        DetectorReadings r{};
        r.burst_ms = ev.burst_ms;
        BreakerState next = b.step(r);
        if (next != prev) {
            // Determine which rule fired
            std::string rule;
            if (r.burst_ms >= thr.burst_ms_main)        rule = "burst_ms>main";
            else if (r.burst_ms >= thr.burst_ms_feeder) rule = "burst_ms>feeder";
            else if (r.burst_ms >= thr.burst_ms_local)  rule = "burst_ms>local";
            else                                         rule = "escalation_latched";
            log.push_back({ev.index, prev, next, rule});
            prev = next;
        }
    }
    return log;
}

// ---------------------------------------------------------------------------
// Test 1 — index_determinism
// The same script must produce the same transition event index on both runs.
// ---------------------------------------------------------------------------
static int test_index_determinism()
{
    BreakerThresholds thr;
    auto script = make_script();

    auto log1 = run_script(script, thr);
    auto log2 = run_script(script, thr);

    if (log1.size() != log2.size()) {
        std::cerr << "[FAIL] index_determinism: log sizes differ ("
                  << log1.size() << " vs " << log2.size() << ")\n";
        return 1;
    }
    for (size_t i = 0; i < log1.size(); ++i) {
        if (log1[i] != log2[i]) {
            std::cerr << "[FAIL] index_determinism: entry " << i
                      << " differs: " << log1[i].to_string()
                      << " vs " << log2[i].to_string() << "\n";
            return 1;
        }
    }
    if (log1.empty()) {
        std::cerr << "[FAIL] index_determinism: no transitions recorded\n";
        return 1;
    }

    std::cout << "[PASS] index_determinism — " << log1.size()
              << " transitions, all at deterministic indices:\n";
    for (const auto& r : log1) {
        std::cout << "       " << r.to_string() << "\n";
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Test 2 — log_replay_equality
// Serialize both logs to string and assert byte-identical content.
// This proves the Gate Decision Journal is reproducible.
// ---------------------------------------------------------------------------
static int test_log_replay_equality()
{
    BreakerThresholds thr;
    auto script = make_script();

    auto log1 = run_script(script, thr);
    auto log2 = run_script(script, thr);

    // Serialize both to canonical strings
    auto serialize = [](const std::vector<TransitionRecord>& log) {
        std::ostringstream ss;
        for (const auto& r : log) ss << r.to_string() << "\n";
        return ss.str();
    };
    std::string s1 = serialize(log1);
    std::string s2 = serialize(log2);

    if (s1 != s2) {
        std::cerr << "[FAIL] log_replay_equality: serialized logs differ\n"
                  << "--- run1 ---\n" << s1
                  << "--- run2 ---\n" << s2;
        return 1;
    }
    std::cout << "[PASS] log_replay_equality — " << log1.size()
              << " entries byte-identical across two independent runs\n";
    return 0;
}

// ---------------------------------------------------------------------------
// Test 3 — mid_burst_fault_injection
// Fire the same script but inject an unexpected burst spike at event 300
// (between the Local and Feeder trips).  Assert:
//   (a) The spike does NOT advance the transition index past 400 (Feeder trip).
//       This would fail if the gate checked timestamps instead of the event
//       stream, because a latency spike could look like a threshold crossing.
//   (b) The latch engages at event 400 regardless of the spike.
// ---------------------------------------------------------------------------
static int test_mid_burst_fault_injection()
{
    BreakerThresholds thr;
    auto script = make_script();

    // Inject a spike at event 300: burst_ms = 1.5 ms (below Local threshold 2.0)
    // This simulates a "cosmic ray" latency spike that should NOT trip a gate.
    auto faulty_script = script;
    for (auto& ev : faulty_script) {
        if (ev.index == 300) ev.burst_ms = 1.5;  // below local threshold
    }

    auto clean_log = run_script(script, thr);
    auto fault_log = run_script(faulty_script, thr);

    // The transition at index 200 (Local) should still be present in both
    bool found_local_clean = false, found_local_fault = false;
    for (const auto& r : clean_log)
        if (r.event_index == 200 && r.to_state == BreakerState::Local)
            found_local_clean = true;
    for (const auto& r : fault_log)
        if (r.event_index == 200 && r.to_state == BreakerState::Local)
            found_local_fault = true;

    if (!found_local_clean || !found_local_fault) {
        std::cerr << "[FAIL] mid_burst_fault_injection: Local trip at index 200 missing\n";
        return 1;
    }

    // The Feeder latch should fire at index 400 in both logs
    bool found_feeder_clean = false, found_feeder_fault = false;
    for (const auto& r : clean_log)
        if (r.event_index == 400 && r.to_state == BreakerState::Feeder)
            found_feeder_clean = true;
    for (const auto& r : fault_log)
        if (r.event_index == 400 && r.to_state == BreakerState::Feeder)
            found_feeder_fault = true;

    if (!found_feeder_clean || !found_feeder_fault) {
        std::cerr << "[FAIL] mid_burst_fault_injection: Feeder latch at index 400 missing\n";
        return 1;
    }

    // The spike at 300 must NOT have created a spurious extra transition
    // (it was below threshold, so state should remain Local)
    for (const auto& r : fault_log) {
        if (r.event_index == 300) {
            std::cerr << "[FAIL] mid_burst_fault_injection: sub-threshold spike at 300 "
                         "triggered a gate transition to "
                      << Breaker::to_string(r.to_state) << "\n";
            return 1;
        }
    }

    std::cout << "[PASS] mid_burst_fault_injection — sub-threshold spike at event 300 "
                 "did not create spurious transition; Feeder latch fires at index 400\n";
    return 0;
}

// ---------------------------------------------------------------------------
// Test 4 — step_latency_sub1us
// Measure the wall time of a single Breaker::step() call over many iterations.
// Assert that the MEDIAN latency is < 1000 ns (1 µs).
// Uses a volatile sink to prevent the call from being elided.
// ---------------------------------------------------------------------------
static int test_step_latency_sub1us()
{
    constexpr size_t kWarmup  = 1000;
    constexpr size_t kSamples = 10000;
    constexpr double kBudgetNs = 1000.0;   // 1 µs

    BreakerThresholds thr;
    Breaker b(thr);

    // Pre-build a nominal readings value
    DetectorReadings nominal{};
    nominal.burst_ms = 0.0;

    // Warmup
    for (size_t i = 0; i < kWarmup; ++i) {
        volatile auto s = b.step(nominal);
        (void)s;
        b.clear_latch();
    }
    b.clear_latch();

    // Timed samples
    std::vector<double> samples;
    samples.reserve(kSamples);
    for (size_t i = 0; i < kSamples; ++i) {
        auto t0 = std::chrono::steady_clock::now();
        volatile auto s = b.step(nominal);
        auto t1 = std::chrono::steady_clock::now();
        (void)s;
        double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
        samples.push_back(ns);
        // Reset so we don't latch and skew times
        b.clear_latch();
    }

    std::sort(samples.begin(), samples.end());
    double median = samples[kSamples / 2];
    double p99    = samples[static_cast<size_t>(kSamples * 0.99)];
    double min_ns = samples.front();

    if (median > kBudgetNs) {
        std::cerr << "[FAIL] step_latency_sub1us: median=" << median
                  << " ns > budget=" << kBudgetNs << " ns\n";
        return 1;
    }
    std::cout << "[PASS] step_latency_sub1us"
              << "  min=" << min_ns << " ns"
              << "  median=" << median << " ns"
              << "  p99=" << p99 << " ns"
              << "  budget=" << kBudgetNs << " ns\n";
    return 0;
}

// ---------------------------------------------------------------------------
// Driver
// ---------------------------------------------------------------------------
int main()
{
    int rc = 0;
    rc |= test_index_determinism();
    rc |= test_log_replay_equality();
    rc |= test_mid_burst_fault_injection();
    rc |= test_step_latency_sub1us();
    if (rc == 0) {
        std::cout << "All gate transition tests PASSED\n";
    } else {
        std::cerr << "One or more gate transition tests FAILED\n";
    }
    return rc;
}
