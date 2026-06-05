# Changelog

## v2.0.1 – Release Hardening (2026-06-04)

### Key changes

- Hardened GitHub Release publishing around canonical `v*` tags for both tag pushes and manual runs.
- Standardized release metadata to read notes from `RELEASE.md` and publish packaged artifacts from `artifacts/release` when present.
- Clarified public packaging and version-branding notes so BQL Engine remains a clean credibility signal for evaluation and reproducibility.

### Notes

- No engine or public API changes.
- This milestone focuses on release hygiene, packaging consistency, and documentation clarity.

---

## Unreleased

### Correctness / Safety

- Hardened `replay` CLI numeric parsing to reject non-finite values (NaN/Inf)
  and negative rates for fault injection flags.
- Hardened Linux `--cpu-pin` parsing/validation and corrected affinity failure
  reporting.
- Added regression coverage for invalid CLI numeric inputs.

## v2.0.0 – Phase 5: Async Proof Pipeline Release (2026-06-03)

### Key changes

- Asynchronous journal processing removed synchronous flushes from the proof pipeline.
- Full deterministic replay now completes in under 1 second with 153 MB/s throughput.
- Tiered performance now measures 1.96M / 1.23M / 1.20M events/sec across match-only, in-process, and full proof modes.
- Release metadata and packaging were updated for BQL 2.00.

### Notes

- No public API changes.
- Existing build, test, and release workflows remain compatible.

---

## v0.6.0 – Phase 4: Async Journal + Throughput Breakthrough (2026-06-02)

### Performance

- **Tier C total runtime:** 60–120s → 0.5–0.9s (~100–200× faster). Full deterministic proof pipeline now
  completes in under 1 second.
- **Journal overhead:** 17.4s → 136ms (~128× faster). Binary audit journal is now written asynchronously.
- **Synchronous flushes:** 100 → 0 (100% eliminated). All journal I/O is non-blocking.
- **Throughput:** 2.1 MB/s → 153 MB/s (~73× higher).

### Throughput by Tier (Phase 4)

| Tier                             | Throughput       | Determinism |
| -------------------------------- | ---------------- | ----------- |
| Tier A — Match-only              | 1.96M events/sec | 100% ✅     |
| Tier B — In-process wire-to-wire | 1.23M events/sec | 100% ✅     |
| Tier C — Full proof pipeline     | 1.20M events/sec | 100% ✅     |

Tier C now runs within 39% of Tier A's raw throughput — the overhead of full deterministic journaling,
digest chaining, and telemetry is effectively near-zero.

### Documentation

- Updated README performance table to reflect Phase 4 measured results across all tiers.
- Added Phase 4 highlights callout explaining journal async rewrite and flush elimination.

---

## v0.5.0 – CI + Security + Observability Hardening (2025-12-02)

### CI & Determinism

- Added repo-wide skip guards (`[skip-ci]` tag and `docs-only` PR label) plus concurrency cancel-in-progress across all workflows to reduce noise and flakiness.
- Upgraded Determinism + CI workflows to surface **p50 / p95 / p99** latency directly in the GitHub job summary and as log notices.
- Standardized benchmark execution to prefer `scripts/run-benchmarks.sh`, with safe fallbacks to `blanc_bench` / `build/bin/blanc_bench`.
- Ensured `bench.jsonl` and `metrics.prom` are collected and uploaded as artifacts for reproducible analysis.
- Enforced **C++17** during determinism builds.

### Security & Supply Chain

- Pinned GitHub Actions to **commit SHAs** repo-wide.
- Added a **Pin Check** workflow to enforce pinned actions and preview updates through scheduled/preview jobs.
- Hardened container scanning with Trivy **v0.67.2**, running filesystem + image scans in non-blocking mode and uploading SARIF results to the GitHub Security tab.
- Emitted compact JSON severity summaries via `jq` for quick scan of findings.
- Fortified `detect-secrets` usage and aligned a guarded baseline refresh workflow.

### Workflow Hygiene & Reporting

- Applied skip guards and concurrency patterns consistently to: CI, Determinism, Container Scan, Reproducibility Check, Detect Secrets, Smoke SITREP, Snapshot Nightly, Verify Bench, Pin Actions Preview/Schedule, Pin Check, and Refresh Secrets Baseline.
- Fixed the Workflow Usage Report wiring and now publish a **“Weekly CI Usage Summary”** issue that aggregates workflow activity over the last 7 days.

### Documentation

- Updated README to describe benchmark surfacing (p50/p95/p99), Trivy configuration, Docker run instructions, and refreshed tooling helper descriptions to match actual workflow behavior.
