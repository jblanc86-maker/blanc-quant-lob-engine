# Blanc Quant LOB Engine v1.0.0

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

---

## 5️⃣ Tiny GitHub Release description (the one-liner + bullets)

For the GitHub “Release” dialog:

**Title:** `v1.0.0 – Stable Deterministic Replay + Tail SLOs`

**Body:**

> First stable release. Deterministic replay, explicit tail-latency gates, hardened CI/security, and automated release packaging.

> - CI + Determinism surface **p50/p95/p99** in job summaries and badges; artifacts captured for historical diffing.
> - Repo-wide **SHA-pinned actions**, Trivy v0.67.2 scanning, and SARIF integration — secure by default, non-blocking visibility.
> - Skip guards + concurrency reduce noise; weekly workflow usage report improves operability.
> - Automated release packaging with rights-marked bundles and optional signatures.

If you paste any of these into your repo, we can do a quick next pass where we tune tone (more “quant infra lab” or more “OSS maintainer”) depending on which audience you’re aiming at.

---

## v1.0.0 Release Notes (Detailed)

## Overview

This is the first stable release of the Blanc Quant LOB Engine. The engine delivers deterministic replay with explicit tail SLO enforcement and a hardened CI + security posture. Public APIs remain compatible, with observability and packaging elevated to production-ready.

### Full Highlights

### 1. CI Quality & Determinism

- Skip guards (`[skip-ci]`, `docs-only` label) and concurrency cancel-in-progress reduce redundant and overlapping runs.
- Determinism + CI workflows now:
  - Enforce modern C++ builds (C++20 target in codebase, workflows previously tested against C++17).
  - Standardize benchmark execution (`scripts/run-benchmarks.sh` with fallbacks).
  - Emit p50 / p95 / p99 latency into `$GITHUB_STEP_SUMMARY` and as notices.
  - Upload `bench.jsonl` and `metrics.prom` artifacts.

### 2. Security & Supply Chain

- All GitHub Actions pinned to commit SHAs where feasible.
- **Pin Check** workflow enforces SHA pinning and proposes updates through preview runs.
- Trivy v0.67.2 runs filesystem and image scans in non-blocking mode, with:
  - JSON severity summaries via `jq`.
  - SARIF results uploaded to the GitHub Security tab.
- Detect-secrets scan is hardened and paired with a guarded baseline refresh workflow.

### 3. Workflow Hygiene & Reporting

- Concurrency and skip guards applied across CI, Determinism, Container Scan, Reproducibility Check, Detect Secrets, Smoke SITREP, Snapshot Nightly, Verify Bench, Pin Actions Preview/Schedule, Pin Check, and Refresh Secrets Baseline.
- Workflow Usage Report now posts/updates a **“Weekly CI Usage Summary”** issue summarizing the last 7 days of GitHub Actions usage.

### 4. Documentation

- README updated to reflect:
  - Benchmark surfacing (p50/p95/p99) in job summaries.
  - Trivy setup and non-blocking behavior.
  - Docker `run` command consistency.
  - Expanded tooling helper descriptions aligned with actual workflows.

## Pre-Release Checklist

Before tagging:

- [ ] Manually dispatch **CI** and **Determinism** workflows and confirm green runs with expected artifacts.
- [ ] Verify the **Security** tab shows the latest SARIF results from the Container Scan workflow.
- [ ] Run **Pin Check** and confirm there are no unpinned actions.
- [ ] Execute release packaging scripts and confirm bundles match expected layout.

Once all checks are green, tag:

```bash
git tag -a v1.0.0 -m "v1.0.0 – Stable deterministic replay + tail SLOs"
git push origin v1.0.0
```

---

## v1.0.0 – Full Release Notes (2025-12-07)

## Major Changes Since Last Release

### CI/CD and Automation
- Hardened all GitHub Actions workflows with explicit permissions blocks for CodeQL compliance and security.
- Added concurrency cancellation, docs-only skip, and [skip-ci] guards to all major workflows.
- Automated pinning of all GitHub Actions to commit SHAs for reproducibility.
- Enhanced artifact upload and release packaging in CI workflows.
- Added dynamic performance badge updates and metrics parsing in CI.
- Improved workflow triggers with paths filters, weekly cron schedules, and manual dispatch.
- Added workflow usage reporting and badge automation.

### Codebase and Build System
- Scaffolded CMakeLists.txt and CMakePresets.json for bql-enterprise module.
- Created header/source stubs for core, determinism, gates, and other modules.
- Added Google Benchmark suite and microbenchmark wiring.
- Improved build scripts for explicit compiler selection and cross-platform support (Linux/macOS).
- Added Boost and nlohmann-json dependencies to build scripts and CI.
- Enhanced Makefile and release packaging scripts.

### Documentation
- Updated README.md with new badges, innovation highlights, and developer setup instructions.
- Added deliverable marking checklist, technology transition appendix, and roadmap.
- Documented Dynamic Execution Gates (DEG) and Release Gates (CI/CD) policy.
- Added comprehensive innovation section and architecture diagrams.
- Linked all new documentation from README.md and docs/.

### Security and Compliance
- Refreshed secrets baseline and improved detect-secrets workflow.
- Added CodeQL and container scan workflows with explicit build steps and artifact uploads.
- Hardened supply chain and license documentation (SPDX, Apache-2.0, BUSL-1.1).
- Added SECURITY.md, CONTRIBUTING.md, and commercial license documentation.

### Testing and Observability
- Added golden-state validation and telemetry-based snapshot tests.
- Enhanced test coverage for book snapshot, burst golden, and release packaging.
- Improved metrics and artifact reporting in CI and determinism workflows.

## Breaking Changes
- All workflows now require explicit permissions blocks.
- All GitHub Actions are pinned to commit SHAs.
- CMake and build scripts require explicit compiler selection.

## Upgrade Notes
- Review new documentation in README.md and docs/ for setup and usage.
- Ensure all local builds use updated CMake and Makefile scripts.
- Review CI workflows for new permissions and artifact handling.

## Contributors
Special thanks to all contributors and reviewers for their work on automation, compliance, and innovation.

---

## Next Steps
- Continue enterprise module population and automation.
- Expand policy, test, and benchmark coverage.
- Harden documentation and release automation for future versions.

---

This release marks v1.0.0 of Blanc Quant LOB Engine with robust automation, compliance, and innovation features.
