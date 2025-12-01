<!-- markdownlint-disable MD013 -->
# Quant LOB Engine

[![CI](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml)
[![Determinism](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml)
[![CodeQL](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)](https://cmake.org/)
[![License: BSL-1.1](https://img.shields.io/badge/License-BSL--1.1-blue.svg)](LICENSE.txt)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/jblanc86-maker/quant-lob-engine/pulls)

Deterministic C++20 limit order book (LOB) replay engine for quantitative and
low-latency research. The project centers on repeatable benches,
patent-pending Dynamic Execution Gates (DEG), and tail SLO enforcement through
`scripts/verify_bench.py`.

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

Gate policy details live in `docs/gates.md`; CI wiring is under
`.github/workflows/verify-bench.yml`.

## Highlights

- Golden digest + tail budgets keep regressions caught early.
- Observability-first artifacts: `bench.jsonl` + `metrics.prom`.
- Conformance + bench scripts plus Prometheus exporter helpers.
- CI ready: determinism, bench, and CodeQL workflows pinned to SHAs.

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

Artifacts land in `artifacts/bench.jsonl` and `artifacts/metrics.prom`.
Deterministic fixtures live under `data/golden/`; regenerate with `gen_synth`
as needed.

## Scripts

```sh
scripts/verify_golden.sh     # digest determinism check
scripts/bench.sh 9           # multi-run benchmark harness
scripts/prom_textfile.sh ... # emit metrics.prom schema
scripts/verify_bench.py      # release gate enforcement
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
artifacts.

## Contributing

See `CONTRIBUTING.md` for workflow expectations. Pull requests should pin new
dependencies, ship matching tests, and update docs for externally visible
changes.

## License

Distributed under the Business Source License 1.1 (`LICENSE.txt`). Research and
non-commercial evaluation are permitted; production use requires a commercial
license until the change date defined in `COMMERCIAL_LICENSE.md`.

<!-- markdownlint-enable MD013 -->
