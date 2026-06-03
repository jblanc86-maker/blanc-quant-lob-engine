# Benchmarks

Blanc Quant LOB Engine publishes deterministic replay evidence, throughput numbers, and CI gate criteria at the repository root so reviewers can verify the core claim quickly: the engine replays the same input deterministically, emits reviewable artifacts, and CI rejects regressions when `p99_ms` breaches budget.

## Current proof points

| Metric | Current result | Why it matters | Source |
| --- | ---: | --- | --- |
| Full proof pipeline runtime | **0.5-0.9s** | Deterministic replay and evidence packaging complete in under a second. | `CHANGELOG.md`, `README.md` |
| Journal path runtime | **136ms** | Audit logging overhead is low enough to keep the proof path practical. | `CHANGELOG.md`, `README.md` |
| Throughput | **153 MB/s** | The proof pipeline sustains high-volume replay without synchronous flush stalls. | `CHANGELOG.md`, `README.md` |
| Tier A throughput | **1.96M events/sec** | Core match-only engine speed. | `CHANGELOG.md`, `README.md` |
| Tier B throughput | **1.23M events/sec** | In-process wire-to-wire throughput. | `CHANGELOG.md`, `README.md` |
| Tier C throughput | **1.20M events/sec** | Full deterministic proof pipeline throughput. | `CHANGELOG.md`, `README.md` |
| CI surfacing badge | **p50/p95/p99 = 16.59/64.05/64.05 ms** | GitHub exposes live benchmark summaries directly from repo badges. | `badges/performance_extra.json` |
| Tail badge | **p99.9/p99.99 ~= 22/24 ms** | Tail metrics are surfaced separately for Tier C review. | `badges/tierc_tail.json` |

## Performance delta vs. prior proof path

| Metric | Before | Current | Improvement |
| --- | ---: | ---: | ---: |
| Tier C runtime | 60-120s | 0.5-0.9s | **~100-200x faster** |
| Journal path | 17.4s | 136ms | **~128x faster** |
| Throughput | 2.1 MB/s | 153 MB/s | **~73x higher** |
| Synchronous flushes | 100 | 0 | **eliminated** |

## CI gate criteria

Release-gate policy is documented in `docs/gates.md` and enforced around machine-verifiable outputs:

- `digest_fnv` must match the golden replay digest.
- `p99_ms` must stay at or below the configured budget.
- `p99.9` requires at least 1,000 samples and `p99.99` requires at least 10,000 samples before those tails are considered valid.
- `bench.jsonl`, `metrics.prom`, and tail-matrix artifacts must be present for review.

## Reproduce locally

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --output-on-failure --test-dir build
make bench
```

Supporting references: `docs/REPRO.md`, `docs/gates.md`, and `artifacts/baselines/sample_runner.json`.
