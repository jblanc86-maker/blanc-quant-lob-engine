# Blanc LOB Engine (v0.9-RC)

[![CI](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml)
[![Determinism](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml/badge.svg?branch=main)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml)
[![Reproducible](https://img.shields.io/badge/reproducible-golden--hash--match-brightgreen)]

[![CodeQL](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml)
[![pre-commit](https://img.shields.io/badge/precommit-passing-blue)]
[![Trivy](https://img.shields.io/badge/Trivy-clean-blue)]

[![C++20](https://img.shields.io/badge/C%2B%2B-20-ff69b4)]
[![CMake+Ninja](https://img.shields.io/badge/CMake-Ninja-informational)]
[![License](https://img.shields.io/badge/license-Apache--2.0-green)](LICENSE)



C++20 market data replay + lightweight order-book signal path. Built with CMake/Ninja, ships simple bench + telemetry export for reproducible runs.

## License model

This project uses **BUSL-1.1 (Business Source License)**.

- Research and non-commercial evaluation are permitted.
- **Production use in live trading or other revenue-generating systems requires a commercial license.**
- On the Change Date, the project converts to a permissive license (Apache-2.0).
  - Change Date: 24 months after the first public release tag of this repository

Benchmarking and publication of results are permitted; please include commit hash, hardware, and methodology details. See `LICENSE` for authoritative terms.

## Commercial use

For production licensing, see `COMMERCIAL_LICENSE.md` or contact your designated licensing channel.

## How it works

```mermaid
flowchart LR
  A[Binary capture<br/>data/golden/itch_1m.bin] --> B[Replay harness<br/>(C++20, cache-friendly)]
  B --> C[Order-book core (SoA)]
  C --> D[Detectors + Breaker gates]
  D --> E[Digest (deterministic)]
  D --> F[Latency metrics p50/p90/p99]
  D --> G[Publish mode summary]
  E --> H[[artifacts/bench.jsonl]]
  F --> I[[artifacts/metrics.prom]]
  G --> J[[stdout run summary]]
```

If Mermaid isn’t rendering on your GitHub preview, it will still render on the repo page (GitHub supports Mermaid). As a fallback, there’s an ASCII alternative in the `docs/` folder if needed.

## Why buy BQS L2

- **Deterministic & auditable:** golden end-state hash + bench JSON/CSV.
- **Tight tails:** we optimize **p99 stability**, not just throughput.
- **Time-to-green:** one-command benches + sample adapters.
- **Hygiene baked in:** pre-commit, sanitizers, secrets baseline, pinned Docker builds.
- **Adaptable core:** SoA book + branch-light parser.
- **Anonymous option:** attribution can be waived in the commercial license.

## Why invest / sponsor

- **Own the roadmap:** fund adapters/features you need.
- **Proof transparency:** reproducible claims with artifacts.
- **Talent magnet:** open, measured performance work attracts systems engineers.

## Build

Prereqs: CMake ≥ 3.20, Ninja, a C++20 compiler (Clang/GCC).

```sh
# Configure (Release) and build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Binaries land in build/bin
ls build/bin/replay
```

Notes

- Compile commands are exported to `build/compile_commands.json` for IDEs.
- Linux Release builds get hardening flags (stack protector, FORTIFY, PIE link) when supported.
- Optional sanitizers for Debug builds:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build -j
```

## Run

```sh
# Show usage
build/bin/replay --help

# Default run (uses data/golden/itch_1m.bin)
build/bin/replay

# Custom input and parameters
build/bin/replay --input path/to/input.bin \
  --gap-ppm 0 --corrupt-ppm 0 --skew-ppm 0 --burst-ms 0
```

Outputs

- `artifacts/bench.jsonl`
- `artifacts/metrics.prom`

## Scripts

```sh
# Conformance (uses scripts/verify_golden.sh)
scripts/verify_golden.sh

# Benchmark (9 iterations by default)
scripts/bench.sh 9

# Export Prometheus textfile metrics
scripts/prom_textfile.sh artifacts/metrics.prom
```

## Security & Safety

## Repository Layout

```text
include/        # headers
src/            # sources (replay, breaker, telemetry)
scripts/        # helper scripts (verify/bench/prom exporter)
artifacts/      # generated outputs (gitignored)
```

## License

See `LICENSE`. Third-party dependencies remain under their respective licenses. If you contribute code, you agree it will be released under this repository's license unless explicitly noted otherwise in the PR description.

### Attribution & Credits

- Core engine & replay implementation © 2025 Blanc contributors.
- Build and CI workflow patterns adapted from open best practices (GitHub Actions hardening, pre-commit hygiene).
- Security scanning integrates detect-secrets (Yelp) and optional CodeQL.
- Docker base image: `ubuntu:24.04` (Canonical).
- Any trademarks or names remain property of their respective owners.

## Troubleshooting

- Build fails with missing Ninja: install Ninja (`brew install ninja`, `apt-get install ninja-build`).
- Oversized input rejected: lower input size or raise `REPLAY_MAX_BYTES`.
- Pre-commit rejects artifacts: ensure `/artifacts/` remains untracked; re-run `git rm -r --cached artifacts` if needed.
