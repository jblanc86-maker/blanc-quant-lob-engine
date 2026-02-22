## Blanc Quant LOB Engine (BQL Engine)

<!-- DO NOT REMOVE: Visitors badge for repo analytics -->

[![p99.9/p99.99](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/jblanc86-maker/blanc-quant-lob-engine/main/badges/tierc_tail.json)](docs/gates.md)
[![p50/p95/p99](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/jblanc86-maker/blanc-quant-lob-engine/main/badges/performance_extra.json)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml)
[![Reproducible](https://img.shields.io/badge/Reproducible-Builds-4caf50.svg)](docs/REPRO.md)
[![Golden-state Deterministic Replay](https://img.shields.io/badge/Golden--state%20Deterministic%20Replay-brightgreen.svg)](docs/gates.md)
[![Determinism](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml)
[![Verify Bench](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/verify-bench.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/verify-bench.yml)
[![CI](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml)
[![Docs Lint](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/docs-lint.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/docs-lint.yml)
[![CodeQL](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml)
[![Container Scan (Trivy v0.67.2)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml)
[![Detect Secrets](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml)
[![Smoke SITREP](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml)
[![Snapshot Nightly](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml)
[![Workflow Usage Report](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/workflow-usage-report.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/workflow-usage-report.yml)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/pulls)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/commits/main)
[![GitHub Release](https://img.shields.io/github/v/release/jblanc86-maker/blanc-quant-lob-engine?include_prereleases)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/releases)
[![CMake + Ninja](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-7952b3.svg)](https://ninja-build.org/)
[![Code Size](https://img.shields.io/github/languages/code-size/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![Top Language](https://img.shields.io/github/languages/top/jblanc86-maker/blanc-quant-lob-engine?logo=cplusplus)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)](https://cmake.org/)
[![Docker](https://img.shields.io/badge/Docker-Enabled-2496ED.svg?logo=docker&logoColor=white)](https://hub.docker.com/)
[![License: BSL-1.1](https://img.shields.io/badge/License-BSL--1.1-blue.svg)](LICENSE)
![Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine)

### About

**Blanc Quant LOB Engine (BQL Engine)** is a **deterministic C++20 replay +
benchmarking harness** for limit-order-book (LOB) workloads, built to answer
one question:

> Can we replay this **exactly**, under load, and prove it didn’t get slower
> (especially at the tails)?

It ships **golden-state determinism checks**, **CI-enforced tail latency
budgets (p50/p95/p99/p99.9/p99.99)**, and **audit-friendly artifacts**
(`bench.jsonl`, `metrics.prom`, HTML report) so regressions fail early and
evidence is reproducible.

**Scope note (important):**

- This repo is the **OSS exercise / benchmark harness** (synthetic + proof
  tooling).
- **BQL 2.0 (Patent-Pending)** is the production-shaped system (real ITCH replay
  - deterministic matching + binary audit journal + coordinated deterministic
    protection + invariance proofs). See the relevant docs sections below.

---

## Performance: Current State vs. Future Targets

| Metric Tier                                                | Current (Jan 2026)                                                          | Target (vNext)                                                                     | Status                  |
| ---------------------------------------------------------- | --------------------------------------------------------------------------- | ---------------------------------------------------------------------------------- | ----------------------- |
| **Tier A: Match-only**<br/>(Core engine speed)             | p50: 1.25μs<br/>p95: 3.29μs<br/>p99: 5.67μs                                 | p50: 100–300μs<br/>p95: 200–600μs<br/>p99: 300–900μs                               | ✅ **EXCEEDS TARGET**   |
| **Tier B: In-process Wire-to-Wire**<br/>(No network/disk)  | Not yet separated                                                           | p50: 0.5–1.5ms<br/>p95: 1–3ms<br/>p99: 2–5ms                                       | 🎯 Planned              |
| **Tier C: Proof Pipeline**<br/>(Full deterministic replay) | p50: ~16ms<br/>p95: ~18ms<br/>p99: ~20ms<br/>p99.9: ~22ms<br/>p99.99: ~24ms | p50: 2–6ms<br/>p95: 4–10ms<br/>p99: 6–15ms<br/>p99.9: ≤3× p99<br/>p99.99: advisory | 🚧 Optimization Phase 2 |
| **Throughput**                                             | 1M events/sec                                                               | 1–5M ops/sec                                                                       | ✅ Baseline Established |
| **Deterministic Replay**                                   | ✅ Verified (100% digest consistency)                                       | ✅ Enhanced with SCM                                                               | ✅ Production Ready     |

> **Tail Latency Purity** — p99.9 and p99.99 are measured on every run (≥1k
> samples for p99.9 and ≥10k samples for stable p99.99). Runs emit `samples`,
> `p999_valid`, and `p9999_valid` to prevent under-sampled tails from being
> misinterpreted. p99.9 is gated at ≤ 3× the p99 budget; p99.99 is advisory.
> Tail-delta gating is validated by `tests/test_tail_latency.cpp`.

---

## Selective Coordination Mode (SCM): Smarter, Deterministic Protection

Selective Coordination Mode brings the “smallest breaker trips first” principle
from power systems into trading engines. Instead of halting everything when
there’s a slowdown, the engine disables or sheds only the affected subsystem —
keeping the rest running and making incident boundaries clean and replayable.

### How It Works

- **Zones:** The engine is divided into protection zones (core match, risk
  checks, telemetry, journaling, snapshotting, adapters).
- **Trip Ladder:** If a zone (like telemetry) gets slow, only that zone is
  tripped first. If the problem persists, the next zone up the ladder is
  tripped, and so on — up to a full halt as a last resort.
- **Coordination Curves:** Each zone has its own latency budget and trip logic
  (e.g., “if p99 latency is breached for M out of N events, trip this zone”).
- **Escalation & Recovery:** The system escalates only if the problem persists,
  and recovers only after a sustained period of good performance (hysteresis).
- **Deterministic Journal:** Every trip, recovery, and action is logged so you
  can replay and audit exactly what happened.

### Coordination Diagram

```text
┌────────────┐   trip   ┌──────────────┐   trip   ┌──────────────┐   trip   ┌────────────┐   trip   ┌───────┐
│ Telemetry  │ ───────▶ │ Journaling   │ ───────▶ │ Risk Checks  │ ───────▶ │ Core Match │ ───────▶ │ HALT  │
└────────────┘          └──────────────┘          └──────────────┘          └────────────┘          └───────┘
```

Gate policy details live in `docs/gates.md`; CI wiring is under
`.github/workflows/verify-bench.yml`.

## Highlights

- Golden digest + explicit tail budgets so regressions fail CI early.
- Observability-first artifacts: `bench.jsonl` + `metrics.prom` for diffing,
  dashboards, and automated SLO checks.
- Conformance + bench scripts are wired for cron / CI, not just local runs.
- CI-ready: determinism, bench, and CodeQL workflows pinned to SHAs.
- Designed to slot into HFT / research pipelines as a replay + guardrail
  module rather than a one-off benchmark toy.

## Build

Prereqs: CMake ≥ 3.20, Ninja, modern C++20 compiler, Boost, and
`nlohmann-json`.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ls build/bin/replay
```

Notes:

- `build/compile_commands.json` aids IDEs.
- Release builds add stack protector, FORTIFY, PIE when supported.
- Enable sanitizers via `-DENABLE_SANITIZERS=ON` on Debug builds.

## Run

```sh
# Default run
build/bin/replay

# Custom input and limits
build/bin/replay --input path/to/input.bin \
  --gap-ppm 0 --corrupt-ppm 0 --skew-ppm 0 --burst-ms 0
```

Artifacts land in `artifacts/bench.jsonl`, `artifacts/metrics.prom`, and the
new HTML analytics dashboard at `artifacts/report/index.html`.
Deterministic fixtures live under `data/golden/`; regenerate with `gen_synth`
as needed.

### Run in Docker

Build the image and run the containerized replay:

```sh
# Build (from repo root)
docker build -t blanc-quant-lob-engine:local .

# Run default golden replay inside the container
docker run --rm blanc-quant-lob-engine:local /app/replay --input /app/data/golden/itch_1m.bin

# Pass a custom file mounted from host
docker run --rm -v "$PWD/data:/data" blanc-quant-lob-engine:local \
  /app/replay --input /data/your_trace.bin
```

## Scripts

```sh
scripts/verify_golden.sh     # digest determinism check
scripts/bench.sh 9           # multi-run benchmark harness
scripts/prom_textfile.sh ... # emit metrics.prom schema
scripts/verify_bench.py      # release gate enforcement
scripts/bench_report.py      # render HTML latency/digest dashboard
```

## Golden-state validation

- Golden digest resides at `data/golden/itch_1m.fnv`.
- `ctest -R golden_state` plus `scripts/verify_golden.sh` ensure
  reproducibility.
- Use `cmake --build build -t golden_sample` (or `make golden`) to refresh
  fixtures after new traces are accepted.

## Developer setup

Ubuntu:

```sh
sudo apt-get update
sudo apt-get install -y cmake ninja-build libboost-all-dev \
  libnlohmann-json3-dev jq
```

macOS:

```sh
brew update
brew install cmake ninja jq nlohmann-json
```

Enable tests with `-DBUILD_TESTING=ON` and run `ctest --output-on-failure -R
book_snapshot` from `build/`. Tests expect `./bin/replay` within the working
directory.

## Release packaging

`./scripts/release_package.sh` creates rights-marked zips plus manifests.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./scripts/release_package.sh --build-dir build --art-dir artifacts \
  --out-dir artifacts/release --git-sha "$(git rev-parse --short HEAD)"
```

Add `--sign` for optional detached GPG signatures. The `snapshot-nightly`
workflow runs this and uploads the bundle automatically.

## Tooling helpers

- `scripts/pin_actions_by_shas.sh` keeps workflow `uses:` entries pinned.
- `.github/workflows/verify-bench.yml` exposes a manual/cron gate run.
- `.github/workflows/determinism.yml` surfaces p50/p95/p99 in the job summary and emits notices for easy viewing.
- `.github/workflows/ci.yml` mirrors bench summary surfacing in the job summary.
- `.github/workflows/container-scan.yml` pins Trivy to v0.67.2, runs fs & image scans non-blocking, and uploads SARIF to the Security tab.
- `docs/technology_transition.md` + `docs/deliverable_marking_checklist.md`
  cover gov delivery and rights-marking guidance.

## CPU pinning (Linux)

```sh
build/bin/replay --input data/golden/itch_1m.bin --cpu-pin 3
# or
CPU_PIN=3 make bench
```

Pinning reduces tail variance on some hosts; measure on your hardware.

## Repository layout

```text
include/        # headers
src/            # replay engine, detectors, telemetry
scripts/        # bench, verify, release, pin helpers
artifacts/      # generated outputs (gitignored)
```

## Security & safety

`SECURITY.md` documents coordinated disclosure. CI integrates detect-secrets
and CodeQL. Signing helpers live under `scripts/` if you need to stamp
artifacts. Blanc LOB Engine is opinionated toward safety-by-default: determinism,
repeatable benches, and explicit tail SLOs are non-negotiable controls rather
than after-the-fact monitoring.

## Contributing

See `CONTRIBUTING.md` for workflow expectations. Pull requests should pin new
dependencies, ship matching tests, and update docs for externally visible
changes.

## License

Distributed under the Business Source License 1.1 (`LICENSE.txt`). Research and
non-commercial evaluation are permitted; production use requires a commercial
license until the change date defined in `COMMERCIAL_LICENSE.md`.

Research users can clone and run the engine today; commercial or production
deployment should follow the terms in `COMMERCIAL_LICENSE.md`.

<!-- markdownlint-enable MD013 -->

## Release Information

This release includes the prebuilt binaries and necessary artifacts for version
1.00 of the Blanc LOB Engine. If you are interested in accessing the full source
code, please reach out directly for further details. The project is fully open
and available for students and hobbyists to explore and use.

## Analytics Report Output

This section documents the HTML analytics report generated by
`scripts/bench_report.py` and visitor tracking integration.

### Generating the Report

Run the benchmark report generator after completing benchmark runs:

```sh
python3 scripts/bench_report.py --bench-file artifacts/bench.jsonl \
  --metrics-file artifacts/metrics.prom --output-dir artifacts/report
```

The report will be generated at `artifacts/report/index.html`.

### Visitor Badge Integration

The repository uses visitor badges to track page views. Badge format:

**Project badge:**

```markdown
![Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine)
```

**Issue-specific badge:**

```markdown
![Issue Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine.issue.<issue_id>)
```

Replace `<issue_id>` with the GitHub issue number.
