# Badges

[![GitHub release](https://img.shields.io/github/v/release/jblanc86-maker/blanc-quant-lob-engine.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/releases)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/pulls)
[![Code Style: clang-format](https://img.shields.io/badge/code%20style-clang--format-brightgreen.svg)](https://clang.llvm.org/docs/ClangFormat.html)
<!-- markdownlint-disable MD013 -->
# Blanc Quant LOB Engine

[![Determinism](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml)
[![CodeQL](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml)
[![Container Scan](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml)
[![Detect Secrets](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml)
[![Smoke SITREP](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml)
[![Snapshot Nightly](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml)

<!-- Performance Badges -->

<!-- Repo & Build Info Badges -->
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)](https://cmake.org/)
[![CMake + Ninja](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-7952b3.svg)](https://ninja-build.org/)
[![Docker](https://img.shields.io/badge/Docker-Enabled-2496ED.svg?logo=docker&logoColor=white)](https://hub.docker.com/)
[![License: BSL-1.1](https://img.shields.io/badge/License-BSL--1.1-blue.svg)](LICENSE)
[![Reproducible](https://img.shields.io/badge/Reproducible-Builds-4caf50.svg)](docs/REPRO.md)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/pulls)
[![Golden-state Deterministic Replay](https://img.shields.io/badge/Golden--state%20Deterministic%20Replay-brightgreen.svg)](docs/gates.md)
![Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine)

Deterministic C++20 limit order book (LOB) replay engine for quantitative and
low-latency research.

Quant LOB Engine is a **replay + benchmarking harness** for HFT-style order
books, built for:

- **Deterministic replay:** Byte-for-byte golden-state checks over ITCH binaries and synthetic bursts.
- **Patent-pending Dynamic Execution Gates (DEG):** Breaker-style gate policies wrap the datapath with explicit safety and tail-latency controls. (Open-source release includes the core breaker state machine; some advanced DEG features remain proprietary.)
- **Tail SLO enforcement:** `scripts/verify_bench.py` treats p50/p95/p99 budgets as release gates, not suggestions.
- **Structured observability:** Every run emits JSONL and Prometheus-compatible textfiles for diffing, dashboards, and CI.

If you care about *"can we replay this exactly, under load, and prove it didn't
get slower or weirder at the tails?"* this engine is the answer.

## What Makes This Innovative

### 1. Golden-State Deterministic Replay

- Guarantees byte-for-byte identical results across runs.
- FNV-1a digest verification: Every replay produces a cryptographic fingerprint of the final order book state.
- Automated dual-run CI: GitHub Actions runs the same input twice and fails if digests differ—catching non-determinism instantly.
- Environment normalization: Fixed timezone, locale, and compiler ensure reproducibility.

### 2. Integrated Determinism + Performance Testing

- Same workflow proves determinism and measures p50/p95/p99 tail latency.
- Release gates enforce SLO budgets: if p99 regresses, CI fails.
- Structured artifacts (`bench.jsonl`, `metrics.prom`) enable historical tracking and automated dashboards.

### 3. Dynamic Execution Gates (Patent-Pending)

- Breaker-style state machine (Fuse → Local → Feeder → Main → Kill).
- Preserves deterministic replay while containing pathological scenarios.
- Explicit publish control: corrupted runs are flagged, not silently trusted.

### 4. Telemetry-Driven Golden-State Validation

- Every run produces machine-readable, CI-auditable artifacts.
- Structured outputs: JSONL event logs + Prometheus textfiles.
- Release gates as code: `scripts/verify_bench.py` treats performance budgets as pass/fail gates.
- Artifact packaging: Automated artifact creation with provenance metadata.

### 5. Canonical Serialization for Order Books

- Structure-of-Arrays (SoA) layout for cache efficiency
- Fixed iteration order regardless of insertion sequence
- FNV-1a rolling hash captures exact state, not approximations

**Why it matters:** Most order books use hash maps or trees that produce
different orderings across runs. Canonical serialization is required for
byte-for-byte reproducibility.

## System architecture

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│   BLANC LOB ENGINE — Deterministic Replay & Benchmark Harness                │
├──────────────────────────────────────────────────────────────────────────────┤
│ Inputs                              Core                           Outputs    │
│ ┌─────────────────┐        ┌──────────────────────────────┐      ┌───────────┐│
│ │ trace_loader    │───▲───▶│ Deterministic Replay         │───┬─▶│ Stdout    ││
│ │ (ITCH bin; CSV/ │   │    │ Scheduler (ST; MT optional)  │   │  │ summary   ││
│ │ PCAP→bin bridge)│   │    └──────────────┬───────────────┘   │  └───────────┘│
│ └─────────────────┘   │                   │                   │              │
│ ┌─────────────────┐   │                   │                   │  ┌───────────┐│
│ │ gen_synth       │───┘   Fault Injection / Gates (DEG‑compatible;         ││
│ │ (synthetic)     │           breaker‑style, optional)           └─▶│ Artifacts ││
│ └─────────────────┘                                     ▲            │ bench.jsonl│
│                                                         │            │ metrics.prom│
│                                         ┌──────────────────────────────┐└───────────┘│
│                                         │ Golden-state Checker         │◀──────┘
│                                         │ (byte-for-byte digest_fnv)   │
│                                         └──────────────────────────────┘
│                                                 │
│                                                 ▼
│ ┌──────────────────────────────┐     ┌──────────────────────────────┐
│ │ Benchmark Harness            │     │ Structured Observability     │
│ │ • msgs/s throughput          │     │ • JSONL event logs           │
│ │ • p50/p95/p99 latency        │     │ • Prometheus textfile        │
│ │ • config matrix sweeps       │     │ • CI artifacts (goldens)     │
│ └──────────────────────────────┘     └──────────────────────────────┘
└──────────────────────────────────────────────────────────────────────────────┘
```

### Flow summary

- ITCH binaries and synthetic `gen_synth` bursts feed a deterministic scheduler
  that enforces DEG-compatible gate policies before emitting telemetry.
- Golden digest checks ensure byte-for-byte stability, while the bench harness
  sweeps configs to publish `bench.jsonl`, Prometheus textfiles, and CI-ready
  artifacts.
- Structured observability (JSONL + textfile) makes it easy to diff runs,
  enforce SLOs, and root-cause tail spikes.
- Dynamic Execution Gates (DEG) model tail behavior as first-class policy,
  making “breaker-style” protections and SLO checks part of the engine instead
  of bolted-on monitoring.

### Classic HFT datapath

```text
┌────────────────────────────────────────────────────────────────────┐
│                    BLANC LOB ENGINE (HFT SYSTEM)                   │
├────────────────────────────────────────────────────────────────────┤
│  ITCH 5.0 parser  ──▶  L2/L3 order book (SoA) ──▶  Price levels    │
│            │                             │                        │
│            ▼                             ▼                        │
│      Dynamic Execution Gates (DEG) ──▶ Telemetry exporter          │
│            │                             │                        │
│            ▼                             ▼                        │
│     gen_synth fixtures          Golden determinism tests          │
└────────────────────────────────────────────────────────────────────┘
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

## Dynamic Execution Gates (provisional patent filed)

Hardware-aware flow control for deterministic replay that uses queue/telemetry signals to keep cache/memory behavior and tail latency reproducible under load.

## Build

Prereqs: CMake ≥ 3.20, Ninja, and a modern C++20 compiler.

See `docs/BEST_PRACTICES.md` for reproducible build and review habits before diving in.

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
sudo apt-get install -y cmake ninja-build jq
```

macOS:

```sh
brew update
brew install cmake ninja jq
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

## Security & Safety

`SECURITY.md` documents coordinated disclosure. CI integrates detect-secrets and CodeQL. Container scanning is handled by Trivy (v0.67.2) with SARIF ingestion to the Security tab and JSON summaries (non-blocking severity gates). Signing helpers live under `scripts/` if you need to stamp artifacts.

Blanc LOB Engine is opinionated toward safety-by-default: determinism, repeatable benches, and explicit tail SLOs are non-negotiable controls rather than after-the-fact monitoring.

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
