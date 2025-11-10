# Blanc LOB Engine (v0.9-RC)

C++20 market data replay + lightweight order-book signal path. Built with CMake/Ninja, ships simple bench + telemetry export for reproducible runs.

![CI](https://github.com/OWNER/REPO/actions/workflows/ci.yml/badge.svg)
[![CodeQL](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml)
[![Security Scan](https://img.shields.io/badge/Secrets--Scan-active-blueviolet.svg)](.github/workflows/secrets-scan.yml)
[![Reproducible Build](https://img.shields.io/badge/Reproducible-Builds-success-green.svg)](.github/workflows/ci.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Docker](https://img.shields.io/badge/Docker-ready-blue.svg)
![Pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen.svg)

## Features

- C++20, single binary (`replay`)
- Replays a binary input file and produces telemetry and a simple breaker state
- Artifacts written to `artifacts/`:
  - `bench.jsonl`: one-line JSON snapshot per run
  - `metrics.prom`: Prometheus textfile metrics
- Helper scripts in `scripts/`:
  - `verify_golden.sh` — conformance check against golden data
  - `bench.sh` — quick p50/p95/p99 timing loop
  - `prom_textfile.sh` — convenience exporter for metrics

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

- Bounded read of input file to mitigate memory exhaustion:
  - Default: 128 MiB
  - Override: `REPLAY_MAX_BYTES=<bytes>`
- Build hardening (Linux Release): `-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`, PIE compile/link when available
- CI/Pre-commit (repository-level): clang-format, codespell, Ruff, detect-secrets
- Artifacts ignored by git (`/artifacts/`) to avoid accidental commits

## Development

Pre-commit hooks

```sh
pip install pre-commit
pre-commit install
pre-commit run --all-files
```

Common CMake options

- `-DCMAKE_BUILD_TYPE=Release|Debug`
- `-DENABLE_SANITIZERS=ON` (Debug; Clang/GCC)

## CI (pipeline)

- GitHub Actions workflow runs pre-commit checks on push/PR
- Recommended next steps (tracked separately):
  - CodeQL code scanning
  - Trivy filesystem/image scanning
  - hadolint for Dockerfiles
  - Pin Actions by commit SHA and enable caching for pip/compilers

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
