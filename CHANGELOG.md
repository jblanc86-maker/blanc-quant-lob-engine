# Changelog

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
