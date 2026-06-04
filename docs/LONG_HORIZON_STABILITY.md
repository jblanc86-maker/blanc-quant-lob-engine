# Long-Horizon Stability Runs

This document defines the **24h stability evidence** required for release-grade readiness.

## Purpose

Long-horizon runs detect drift, thermal throttling, kernel jitter, or slow tail
creep that short CI benches cannot reveal. The run captures both tail-latency
metrics and gate behavior across time.

## Workflow

`.github/workflows/long_horizon_stability.yml`

- Runs only on **self-hosted** runners (24h exceeds GitHub-hosted limits).
- Builds `replay` + `gen_synth`, then loops for the configured duration.
- Default settings: `DURATION_HOURS=24`, `INTERVAL_SEC=300` (5 minutes).

## Script

`scripts/long_horizon_stability.sh`

- Writes artifacts under `artifacts/stability/`
- Captures:
  - `run_N/replay_stdout.txt`
  - `run_N/bench.jsonl` (if present)
  - `run_N/metrics.prom`
  - `stability_summary.jsonl` (timestamped JSONL with p50/p95/p99/p99.9/p99.99)

## Quick smoke run (recommended)

Before starting a long (multi-hour) run, it’s useful to validate that the harness
produces evidence artifacts end-to-end.

The stability script supports a bounded iteration mode via `MAX_ITERS`:

```bash
# One iteration, minimal wait
DURATION_HOURS=0 MAX_ITERS=1 INTERVAL_SEC=1 \
  REPLAY_BIN=build/bin/replay INPUT_BIN=data/golden/itch_1m.bin \
  ./scripts/long_horizon_stability.sh

# Expect: artifacts/stability/stability_summary.jsonl (non-empty)
```

This is especially handy when editing CI wiring or when verifying that the
“long-horizon stability evidence present” gate will have something to upload.

## Evidence expectations

A release-grade run includes:

- 24h continuous coverage
- Stable digest values (no divergence)
- No widening trend in p99 / p99.9 / p99.99
- No unexpected breaker escalation patterns

## Example summary record

```json
{
  "timestamp": "2026-02-21T16:30:00Z",
  "iteration": 12,
  "metrics": {
    "p50_ms": 0.011,
    "p95_ms": 0.034,
    "p99_ms": 0.049,
    "p999_ms": 0.061,
    "p9999_ms": 0.072,
    "digest_fnv": "0x…",
    "determinism": true,
    "cpu_pin": 3
  }
}
```

## Release checklist

- [ ] Stability artifact uploaded in the release tag run
- [ ] `stability_summary.jsonl` attached to evidence bundle
- [ ] Reviewer sign-off on 24h tail trend

---

Last updated: 2026-02-21
