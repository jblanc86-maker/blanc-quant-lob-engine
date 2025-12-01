# Dynamic Execution Gates (Runtime) and Release Gates (CI/CD)

This repository distinguishes between two complementary gate concepts:

## Dynamic Execution Gates (runtime)

Shape pathological flows during replay to keep tails tight and preserve determinism.

- Pros: contain bursts/skews without blowing up p99; great for stress scenarios.
- Cons: add complexity; can bias results if you don’t log/justify activations.

Recommended runtime interface (planned):

- `--gate-profile {off,strict,soft}`
- `--gate-max-burst-ms <int>`
- `--gate-gap-ppm <int>`

Runtime metrics (Prometheus/textfile):

- `gate_activations_total`
- `gate_suppressed_msgs_total`
- `gate_tail_delta_ms{phase=...,p="p99"}`

Every activation should be logged with: timestamp, rule id, input window
stats, before/after tail deltas.

## Release Gates (CI/CD)

Block merges/releases unless determinism and performance budgets hold.

- Pros: stop regressions cold; ensure byte-for-byte reproducibility and
  latency SLOs.
- Cons: need good thresholds and fixtures; too‑tight budgets can slow iteration.

Release checks (inputs typically produced by benchmark jobs):

- `digest_fnv` must match golden
- `p99_ms` ≤ budget (e.g., `0.50 × baseline + epsilon`)
- Artifacts present and schema‑checked: `bench.jsonl`, `metrics.prom`

## Recommended defaults (practical)

- Golden runs (research, CI determinism): gates OFF. Enforce via release gates:
  - `digest_fnv == GOLDEN`
  - `p99_ms ≤ BUDGET`
  - `bench.jsonl` + `metrics.prom` present and schema‑valid
- Stress/robustness runs: gates ON (explicit) with parameters pinned in config:
  - Log every activation (timestamp, rule id, window stats, tail deltas)
  - Assert "no tail widening" invariant (reject run if p99↑ and gate was active)
  - Clearly label outputs as “gated” vs “ungated” for reviewers

## Separation of concerns

Store baselines separately:

- Ungated: `artifacts/golden/...`
- Gated: `artifacts/stress/...`

## Bottom line

- Dynamic execution gates keep runs behaving under adversarial inputs.
- Release gates keep the codebase honest across time.
- Run ungated for truth, gated for resilience — ship only when both pass.

## Tooling

- `scripts/verify_bench.py` enforces the policies above using `bench.jsonl`,
  `metrics.prom`, and the per-runner baselines under `artifacts/baselines/`.
- `.github/workflows/verify-bench.yml` wires the script into a
  `workflow_dispatch` entry point so you can run gates on demand (or clone the
  workflow for nightly CI).
