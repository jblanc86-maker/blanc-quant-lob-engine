# Contributing to Blanc LOB Engine

We keep patches boring, testable, and reversible. This guide explains the workflow, tooling, and quality bar we expect.

## Quick Start

```bash
git switch -c feat/<slug>
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --output-on-failure --test-dir build || true
pre-commit run -a
```

## Development Environment

* CMake ≥ 3.20, Ninja, and a C++20 compiler (Clang/GCC).
* Boost (currently program_options) and standard build tools.
* Python 3.11+ for scripts and pre-commit hooks.
* Optional: Docker (see `Dockerfile`) for hermetic builds.

## Build & Test

Release build + smoke tests:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --output-on-failure --test-dir build
```

Debug with sanitizers:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build -j
ctest --output-on-failure --test-dir build
```

Scripts for quick checks:

```sh
scripts/verify_golden.sh   # golden data conformance
scripts/bench.sh           # latency sample run
scripts/prom_textfile.sh   # export Prometheus metrics
```

## Pre-commit Hooks

We rely on pre-commit for hygiene (whitespace, YAML/JSON/TOML, clang-format,
codespell, detect-secrets, hadolint).

```sh
pip install pre-commit
pre-commit install
pre-commit run -a
```

## Coding Standards

* C++20, clang-format enforced.
* Keep functions short and deterministic.
* Include `SPDX-License-Identifier: Apache-2.0` at the top of all new source/scripts.

## Commit Messages & PRs

* Use conventional prefixes (`feat:`, `fix:`, `docs:`, `ci:`, `chore:`).
* 72-character subject line, details in body.
* PRs must describe motivation + testing notes, link issues, and stay focused.

## Tests & CI Expectations

* Every PR should add/adjust at least one test (unit or smoke) when behavior changes.
* `ctest` must be green; CI (Ubuntu/macOS) must pass before merge.

## Security & Secrets

* Never commit credentials. Detect-secrets baseline runs in CI.
* Report vulnerabilities privately—see `SECURITY.md`.

## License & DCO

* By contributing, you license your code under Apache-2.0.
* Include a DCO sign-off in each commit (`git commit -s`).

Example sign-off:

```text
Signed-off-by: Your Name <your.email@example.com>
```

Thanks again for helping improve Blanc LOB Engine!
