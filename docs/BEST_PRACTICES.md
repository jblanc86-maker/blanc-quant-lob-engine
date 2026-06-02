# Best Practices

These tips capture the recurring setup and review habits that keep the engine deterministic and healthy.

## Build and toolchain

- Use the pinned CMake/Ninja toolchain and a modern C++20 compiler; keep `CMAKE_BUILD_TYPE` consistent across runs to preserve determinism.
- Prefer Release builds (`-DCMAKE_BUILD_TYPE=Release`) when validating latency or throughput; add `-DENABLE_SANITIZERS=ON` on Debug builds to surface UB early.
- Regenerate `build/compile_commands.json` when the build directory changes so IDEs and static analyzers stay in sync.
- Run `clang-format` before committing to avoid noisy diffs.

## Determinism and benchmarking

- Use the golden fixtures under `data/golden/` to confirm byte-for-byte stability before broader changes.
- For performance regressions, run `scripts/verify_bench.py` so p50/p95/p99 thresholds remain enforced.
- Keep the environment stable (timezone, locale, compiler version) across benchmark runs to avoid artificial drift.

## Testing and CI hygiene

- Execute the determinism and smoke workflows locally when possible before pushing; they mirror the CI gates.
- Review `artifacts/bench.jsonl` and `artifacts/metrics.prom` outputs after runs to catch tail spikes early.
- Avoid introducing non-deterministic constructs (unordered containers without stable iteration, random seeds without explicit values).

## CPU pinning

- **Do not pin to core 0.** Core 0 is the default target for OS interrupt affinity
  (`/proc/irq/*/smp_affinity`), NIC softirqs, RCU callbacks, and timer wheel events.
  Sharing it with the replay engine injects unpredictable latency spikes.
- **Use `--cpu-pin 3`** (or the first non-sibling, non-interrupt core on your machine).
  Core 3 avoids core 0 interrupts, avoids the SMT sibling of core 0 (typically core 1),
  and is not the last core (often used for watchdog threads).
- Verify your choice: `cat /proc/irq/*/smp_affinity_list | sort -u` to see where
  interrupts land; `cat /sys/devices/system/cpu/cpu*/topology/thread_siblings_list`
  to see SMT pairs.
- The `cpu_pin` field is emitted in every JSONL and Prometheus artifact — a run with
  `cpu_pin: 0` should be flagged as potentially noise-contaminated.
- See `docs/CPU_PINNING_RATIONALE.md` for the full core-selection rationale and NUMA
  topology guidance.

## Documentation and safety

- Document new gate policies or replay behaviors in `docs/` so CI reviewers can trace intent.
- Keep secrets out of the repo; the Detect Secrets workflow will fail builds when incidental tokens are committed.
