# Blanc Quant LOB Engine v0.5.0

Release date: 2025-12-02

## Highlights

- CI/Determinism performance visibility
  - Bench metrics (p50/p95/p99) surfaced in job summary and as notices.
  - Robust benchmark execution in Determinism: prefers `scripts/run-benchmarks.sh`, falls back to `blanc_bench` binary.
  - Artifacts collected reliably: `bench.jsonl`, `metrics.prom`, summary JSON.

- Workflow hygiene & noise reduction
  - Concurrency with cancel-in-progress across workflows.
  - Skip guards for `"[skip-ci]"` commit tag and `docs-only` PR label.
  - Manual dispatch and weekly schedules aligned where useful.

- Security & action pinning
  - Actions pinned to commit SHAs repo-wide; Pin Check enforces pins.
  - Preview/scheduled pin workflows propose safe pin updates via PRs.
  - CodeQL and Detect-Secrets wired with proper permissions.

- Container scanning improvements
  - Trivy pinned to v0.67.2 via setup action.
  - Filesystem and image scans run non-blocking (exit-code '0').
  - SARIF uploaded to GitHub Security tab; JSON severity summaries in logs.

- Documentation
  - README updated to reflect bench summary surfacing and Trivy details.
  - Docker run instructions consolidated around `/app/replay`.

## Breaking changes

- None. Public APIs and build interfaces remain compatible.

## Upgrade notes

- No special steps required. CI will surface tail latency metrics automatically.
- Ensure runners have jq available (installed in workflows) for summary parsing.

## Checksums / Artifacts

- CI and Determinism upload benchmark artifacts and release bundles when applicable.

## Acknowledgements

Thanks to contributors and reviewers for CI/security hardening and observability improvements.
