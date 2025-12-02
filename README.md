<!-- markdownlint-disable MD013 -->
# Blanc Quant LOB Engine

[![CI](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml)
[![Determinism](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml)
[![CodeQL](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml)
[![Container Scan](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml)
[![GitHub Release](https://img.shields.io/github/v/release/jblanc86-maker/blanc-quant-lob-engine?include_prereleases)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/releases)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/commits/main)
[![Code Size](https://img.shields.io/github/languages/code-size/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![Top Language](https://img.shields.io/github/languages/top/jblanc86-maker/blanc-quant-lob-engine?logo=cplusplus)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)](https://cmake.org/)
[![CMake + Ninja](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-7952b3.svg)](https://ninja-build.org/)
[![Docker](https://img.shields.io/badge/Docker-Enabled-2496ED.svg?logo=docker&logoColor=white)](https://hub.docker.com/)
[![License: BSL-1.1](https://img.shields.io/badge/License-BSL--1.1-blue.svg)](LICENSE.txt)
[![Reproducible](https://img.shields.io/badge/Reproducible-Builds-4caf50.svg)](docs/REPRO.md)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/jblanc86-maker/quant-lob-engine/pulls)
[![Golden-state Deterministic Replay](https://img.shields.io/badge/Golden--state%20Deterministic%20Replay-brightgreen.svg)](docs/feature_specs.md)
[![GCC/Clang](https://img.shields.io/badge/Compiler-GCC%2013%2B%20%7C%20Clang%2010%2B-yellow.svg)](https://isocpp.org/)
[![Linux](https://img.shields.io/badge/Linux-OS-333.svg?logo=linux)](https://www.kernel.org/)
[![Ubuntu](https://img.shields.io/badge/Ubuntu-20.04%2B-e95420.svg?logo=ubuntu)](https://ubuntu.com/)
[![macOS](https://img.shields.io/badge/macOS-12%2B-007aff.svg?logo=apple)](https://www.apple.com/macos/)
![Issue Visitors](https://visitor-badge.laobi.icu/badge?page_id=blanc86-maker.blanc-quant-lob-engine.issue.18)

Deterministic C++20 limit order book (LOB) replay engine for quantitative and
low-latency research.

Quant LOB Engine is a **replay + benchmarking harness** for HFT-style order
books, built for:

- **Deterministic replay:** byte-for-byte golden-state checks over ITCH binaries
  and synthetic bursts.
- **Patent-pending Dynamic Execution Gates (DEG):** breaker-style gate policies
  that wrap the datapath with explicit safety and tail-latency controls. *Note:
  The open-source release includes the core breaker state machine; some advanced
  DEG features remain proprietary.*
- **Tail SLO enforcement:** `scripts/verify_bench.py` treats p50/p95/p99 budgets
  as **release gates**, not suggestions.
- **Structured observability:** every run emits JSONL + Prometheus-compatible
  textfiles for diffing, dashboards, and CI.

If you care about *“can we replay this exactly, under load, and prove it didn’t
get slower or weirder at the tails?”* this engine is the answer.

## System architecture

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│   QUANT LOB ENGINE — Deterministic Replay & Benchmark Harness                │
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
│                    QUANT LOB ENGINE (HFT SYSTEM)                   │
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
artifacts. Quant LOB Engine is opinionated toward safety-by-default: determinism,
repeatable benches, and explicit tail SLOs are non-negotiable controls rather
than after-the-fact monitoring.

## Contributing

See `CONTRIBUTING.md` for workflow expectations. Pull requests should pin new
dependencies, ship matching tests, and update docs for externally visible
changes.

## License

Distributed under the Business Source License 1.1 (`LICENSE.txt`). Research and
non-commercial evaluation are permitted; production use requires a commercial
Research users can clone and run the engine today; commercial or production
deployment requires a license until the change date in
`COMMERCIAL_LICENSE.md`.

<!-- markdownlint-enable MD013 -->

## Analytics Report Output

This section is reserved for the HTML analytics report generated by
`scripts/bench_report.py`.

To enable visitor/location tracking in your report, add the following Google
Analytics snippet to your HTML output:

```html
<!-- Google Analytics -->
<script async src="https://www.googletagmanager.com/gtag/js?id=G-XXXXXXXXXX"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag() { dataLayer.push(arguments); }
  gtag('js', new Date());
  gtag('config', 'G-XXXXXXXXXX');
</script>
```

Replace `G-XXXXXXXXXX` with your Google Analytics 4 measurement ID
(or use `UA-XXXXXX-X` format for Universal Analytics properties).
