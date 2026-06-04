# Roadmap — Blanc LOB Engine

This roadmap describes the planned phases for the project, the scope of each phase, and success criteria. It frames the technical milestones, tests, and CI required to deliver a reproducible, auditable L2 replay engine suitable for research, benchmarking, and controlled commercial usage.

---

## OSS vs. Enterprise Split

**Public BQL Engine** is the reproducible OSS benchmark and proof harness.
It provides the deterministic replay core, CI-enforced performance contracts,
provenance artifacts, and audit evidence — everything you need to evaluate,
verify, and integrate the engine.

**Enterprise BQL 2.0** packages the production adapters, deployment
integrations, dashboards, support, and commercial terms on top of the OSS core.

---

## What is available now (Public BQL Engine)

These capabilities are live in the current repository and CI pipeline:

| Capability                                                       | Status                                     |
| ---------------------------------------------------------------- | ------------------------------------------ |
| Reproducible benchmark reports (CI-generated, artifact-retained) | ✅ Available today                         |
| Evidence bundles with SHA-256 hashes and provenance metadata     | ✅ Available today                         |
| CI/CD integration templates (GitHub Actions, CMake/Ninja gates)  | ✅ CI-ready today                          |
| Prometheus textfile metrics (`artifacts/metrics.prom`)           | ✅ Available today                         |
| Commercial license for production deployment                     | ✅ Available — see `COMMERCIAL_LICENSE.md` |
| 365-day CI artifact retention for compliance and patent evidence | ✅ Available today                         |

---

## Enterprise BQL 2.0 — Roadmap

Enterprise BQL 2.0 adds the following on top of the public engine:

- **Real ITCH/FIX ingestion adapters** — production market-data feed connectors
  replacing the synthetic harness for live and co-located deployments.
- **Prometheus/Grafana observability dashboards** — full dashboard package for
  latency, throughput, and SLO tracking across tiers (Prometheus textfile
  metrics are available in the OSS engine today; the Grafana dashboard package
  ships with Enterprise BQL 2.0).
- **CI/CD integration templates for latency gates** — hardened enterprise
  templates with multi-environment matrix builds, regression budgets, and
  automated PR-level benchmark comments.
- **Enterprise auth/SSO deployment layer** — SAML/OIDC integration,
  role-based access, and audit-log forwarding for regulated environments.
- **Reproducible benchmark reports and artifact retention policies** — extended
  retention, tamper-evident packaging, and structured export for legal/compliance.
- **Legal/compliance evidence bundles** — complete bundles with hashes,
  provenance, chain-of-custody metadata, and formatted submission artifacts for
  patent, regulatory, and audit workflows.
- **Pilot support and reference deployment program** — dedicated onboarding,
  design-partner engagement, and reference architecture review.
- **Commercial license, support terms, and integration guidance** — SLA-backed
  support, integration consulting, and production-deployment guidance.

> If you are interested in Enterprise BQL 2.0, open an issue with the label
> `enterprise` or contact us as described in `COMMERCIAL_LICENSE.md`.

---

## Development Phases

### Phase 1 — Deterministic replay & CORE telemetry (COMPLETE)

- Features: deterministic replay engine (digest), bench harness, telemetry JSONL + Prometheus metrics, pin-by-SHA workflows, safe pin script (dry-run + preview PR), CPU pin support, basic unit tests, golden validation.
- Success criteria: CI builds + ctest pass; golden FNV verification test; bench JSONL + metrics produced and uploaded by CI for runs.

### Phase 2 — Full-order-book snapshots & granular invariants (COMPLETE)

- Features: full Book serialization API, book snapshot & per-message state output, golden-book snapshots for regression tests.
- Goals: deterministic snapshot equality checks across runs, minimal per-message deltas, and test harnesses to re-check books against a golden set.
- Success criteria: book snapshot CTest passes in CI; acceptance that changes to the Book are guarded by snapshot diffs; snapshot tests for various golden inputs (normal, burst, corrupted).

### Phase 3 — CI-driven regression monitoring & PR-level benchmarks (COMPLETE)

- Features: per-run bench artifact upload; benchmark delta reports on PRs; an automated daily baseline and comparison; a nightly job to produce aggregated metrics and summarize drift.
- Goals: reduce nonfunctional regressions and provide PR reviewers with clear p50/p95/p99 deltas.
- Success criteria: multi-run baselines and PR comments with delta metrics; allowed thresholds and automated alerts.

### Phase 4 — Profiling, hotspots & performance exercises (CURRENT)

- Features: integrated profiling workflows (perf, eBPF), annotated hotspots on Flamegraphs, scaling & NUMA-aware tests, LTO/arch flags matrix, stress/gating tests.
- Goals: identify hotspots, reduce p99, and validate scaling to cores using CPU affinity and worker thread pinning.
- Success criteria: measured improvements (p95/p99) via CI runs; documentation for core tuning; <= 50% p99 variance across runs with CPU pinning and hardware parity.

### Phase 5 — Production readiness & commercial packaging (NEXT)

- Features: hardened build pipelines (reproducible build artifacts), license/COM-Lic packaging, commercial benchmarking reports, enterprise adapters (FIX/MD API), monitoring integrations.
- Goals: meet licensing, security, and operational standards for commercial deployments, and provide a migration kit & documentation.
- Success criteria: product-grade docs, enterprise release candidates, and an audited license/compliance check.

---

## Cross-cutting activities & notes

- Observability traces: instrument and export additional internal traces for deep debugging (optional; consider privacy & perf costs).
- Security & licensing: ensure third-party license audit, include `COMMERCIAL_LICENSE.md` & `LICENSE` usage guidance.
- Benchmarks & runtime: standardize hardware reporting (CPU family, memory) in CI for apples-to-apples comparisons.

---

## Owners & timelines

- Owner: core engineering (Jean Blanc)
- Cadence: 1–2 week sprints with milestone gating via CI benchmarks and deterministic replay checks.

## Contact

- For questions about roadmap or productization, open an issue with the label `roadmap`.
- For Enterprise BQL 2.0 inquiries, open an issue with the label `enterprise` or see `COMMERCIAL_LICENSE.md`.
