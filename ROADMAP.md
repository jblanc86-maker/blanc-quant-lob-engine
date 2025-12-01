# Roadmap — Blanc LOB Engine

This roadmap describes the planned phases for the project, the scope of each phase, and success criteria. It frames the technical milestones, tests, and CI required to deliver a reproducible, auditable L2 replay engine suitable for research, benchmarking, and controlled commercial usage.

Phases and milestones

Phase 1 — Deterministic replay & CORE telemetry (COMPLETE)

- Features: deterministic replay engine (digest), bench harness, telemetry JSONL + Prometheus metrics, pin-by-SHA workflows, safe pin script (dry-run + preview PR), CPU pin support, basic unit tests, golden validation.
- Success criteria: CI builds + ctest pass; golden FNV verification test; bench JSONL + metrics produced and uploaded by CI for runs.

Phase 2 — Full-order-book snapshots & granular invariants (IN-PROGRESS)

- Features: full Book serialization API, book snapshot & per-message state output, golden-book snapshots for regression tests.
- Goals: deterministic snapshot equality checks across runs, minimal per-message deltas, and test harnesses to re-check books against a golden set.
- Success criteria: book snapshot CTest passes in CI; acceptance that changes to the Book are guarded by snapshot diffs; snapshot tests for various golden inputs (normal, burst, corrupted).

Phase 3 — CI-driven regression monitoring & PR-level benchmarks (NEXT)

- Features: per-run bench artifact upload; benchmark delta reports on PRs; an automated daily baseline and comparison; a nightly job to produce aggregated metrics and summarize drift.
- Goals: reduce nonfunctional regressions and provide PR reviewers with clear p50/p95/p99 deltas.
- Success criteria: multi-run baselines and PR comments with delta metrics; allowed thresholds and automated alerts.

Phase 4 — Profiling, hotspots & performance exercises

- Features: integrated profiling workflows (perf, eBPF), annotated hotspots on Flamegraphs, scaling & NUMA-aware tests, LTO/arch flags matrix, stress/gating tests.
- Goals: identify hotspots, reduce p99, and validate scaling to cores using CPU affinity and worker thread pinning.
- Success criteria: measured improvements (p95/p99) via CI runs; documentation for core tuning; <= 50% p99 variance across runs with CPU pinning and hardware parity.

Phase 5 — Production readiness & commercial packaging

- Features: hardened build pipelines (reproducible build artifacts), license/COM-Lic packaging, commercial benchmarking reports, enterprise adapters (FIX/MD API), monitoring integrations.
- Goals: meet licensing, security, and operational standards for commercial deployments, and provide a migration kit & documentation.
- Success criteria: product-grade docs, enterprise release candidates, and an audited license/compliance check.

Cross-cutting activities & notes

- Observability traces: instrument and export additional internal traces for deep debugging (optional; consider privacy & perf costs).
- Security & licensing: ensure third-party license audit, include `COMMERCIAL_LICENSE.md` & `LICENSE` usage guidance.
- Benchmarks & runtime: standardize hardware reporting (CPU family, memory) in CI for apples-to-apples comparisons.

Owners & timelines (proposal)

- Owner: core engineering (Jean Blanc) — Phase 1/2
- Proposed sprint plan: 1–2 week sprints; Phase 2 (book snapshots) baked into the current milestone.

Contact

- For questions about roadmap or productization, open an issue with the label `roadmap`.
