# Blanc LOB Engine (v0.9-RC)

[![CI](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/ci.yml)
[![Determinism](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml/badge.svg?branch=main)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/determinism.yml)
[![Reproducible](https://img.shields.io/badge/reproducible-golden--hash--match-brightgreen)]

[![CodeQL](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/quant-lob-engine/actions/workflows/codeql.yml)
[![pre-commit](https://img.shields.io/badge/precommit-passing-blue)]
[![Trivy](https://img.shields.io/badge/Trivy-clean-blue)]

[![C++20](https://img.shields.io/badge/C%2B%2B-20-ff69b4)]
[![CMake+Ninja](https://img.shields.io/badge/CMake-Ninja-informational)]
[![License](https://img.shields.io/badge/license-Apache--2.0-green)](LICENSE)]

C++20 market data replay + lightweight order-book signal path. Built with CMake/Ninja, ships simple bench + telemetry export for reproducible runs.

Blanc LOB Engine is a functionally deterministic C++20 Level-2 order-book replay and benchmarking framework built with CMake/Ninja for repeatable, auditable low-latency research. It emphasizes reproducible golden state validation, tight tail latency (p50 / p95 / p99), and stable throughput across runs, with first-class observability via structured artifacts (JSONL and Prometheus-compatible metrics). The system includes patent-pending breaker-style gates. Trading use subject to license terms.

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
  D --> F[Latency metrics p50/p95/p99]
  D --> G[Publish mode summary]
  E --> H[[artifacts/bench.jsonl]]
  F --> I[[artifacts/metrics.prom]]
  G --> J[[stdout run summary]]
```

If Mermaid isn’t rendering on your GitHub preview, it will still render on the repo page (GitHub supports Mermaid). As a fallback, there’s an ASCII alternative in the `docs/` folder if needed.

## Why buy BQS L2

- **Deterministic & auditable:** golden end-state hash + bench JSON/CSV.
- **Tail metrics:** p50/p95/p99 latency + throughput.
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

### Golden-state validation

The project includes a golden-state check based on a functionally deterministic digest produced by the replay run. The expected FNV digest is kept in `data/golden/itch_1m.fnv`; the `scripts/verify_golden.sh` script and the `golden_state` CTest validate this value. To regenerate golden files from a synthetic trace, use `make golden`.

Note: the current "book snapshot" tests are telemetry-based: the test reads `bench.jsonl` and verifies a set of telemetry fields (digest, publish flag, breaker state, and detector/readings). Full per-orderbook serialization (a byte-for-byte L2 book snapshot) is tracked as Phase 2 in the roadmap and will provide deeper, per-level state diffs once implemented.

## Tools — pin GitHub Actions

There's a small helper `scripts/pin_actions_by_shas.sh` to pin `uses:` entries in `.github/workflows` to commit SHAs.

Usage:

```sh
# Preview (dry-run): lists proposed changes without editing files
./scripts/pin_actions_by_shas.sh --dry-run --output-json pin_proposals.json

# Apply to files (makes in-place changes):
./scripts/pin_actions_by_shas.sh
```

There's a preview workflow `.github/workflows/pin-actions-preview.yml` to run the script and create a draft PR with pinned changes (run via 'Actions' -> 'Pin Actions Preview').

## CPU pinning (Linux-only)

You can optionally pin the replay process to a CPU core to reduce scheduling noise and improve determinism during benchmarks. This is a best-effort, Linux-only feature.

Usage examples:

```sh
# Pin to CPU 3 via the CLI
build/bin/replay --input data/golden/itch_1m.bin --cpu-pin 3

# Or use the Makefile convenience wrapper; pass an integer core id into `CPU_PIN`:
CPU_PIN=3 make bench
```

Notes:


CPU pinning effects (initial observations):
In limited local testing (5× runs per configuration), CPU pinning slightly reduced mean p50 latency and, in some cases, significantly reduced tail latency variability (p95 standard deviation). Absolute p95 values varied modestly by core selection. These results suggest CPU pinning can improve repeatability of tail measurements under certain conditions. Additional runs across longer durations and multiple hardware instances are required for statistical confidence.

### Preview PR bench metrics

Preview PRs created by the `pin-actions-preview.yml` workflow include a bench step that captures `bench.jsonl` and `metrics.prom` for the preview branch. The workflow posts a summary comment on the PR with p50/p95/p99 metrics and a link to the artifacts; this makes reviewing performance/regressions easier when pinning actions.

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
