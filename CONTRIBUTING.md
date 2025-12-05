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
* Standard build tools (CMake + Ninja) and a modern C++20 compiler.
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

---

## For Newcomers

Welcome! Here are some tips to help you get started contributing:

### How to Submit Issues

* Report bugs or request features via [GitHub Issues](https://github.com/jblanc86-maker/quant-lob-engine/issues).

* Please include steps to reproduce, expected behavior, and relevant logs or screenshots.

### Code Review Process

* All pull requests are reviewed by maintainers or experienced contributors.

* You may be asked to revise code for clarity, style, or test coverage.
* Reviews are typically completed within a few days.

### Branching Model

* Create feature branches from `main` using `git switch -c feat/<slug>`.

* Use descriptive branch names (e.g., `fix/latency-bug`, `docs/update-readme`).

### Writing & Locating Tests

* Add or update tests in the `tests/` directory for C++ and Python code.

* Aim for clear, focused tests that cover new or changed behavior.
* Use `ctest` for C++ and `pytest` for Python scripts if applicable.

### CI Details

* All pull requests trigger CI checks (lint, build, test, determinism).

* If CI fails, review logs and fix issues locally before pushing updates.
* You can run most checks locally using the scripts in `scripts/`.

### Documentation Standards

* Update relevant documentation (README, code comments, docstrings) when changing features or APIs.

* Well-documented code is easier for everyone to use and maintain.

### Contact & Community

* For questions, open a GitHub Discussion or email the maintainer listed in `SECURITY.md`.

* We welcome all constructive feedback and ideas!

### Code of Conduct

* Please review our [CODE_OF_CONDUCT.md](./CODE_OF_CONDUCT.md) and treat all contributors with respect.

### Python Style Guide

* For Python scripts, follow [PEP8](https://peps.python.org/pep-0008/) and use `ruff` for linting/formatting.

* Run `pre-commit run -a` before committing changes.

### Release Process

* Releases are managed by maintainers, but contributors can help by updating changelogs and testing release candidates.

* See `RELEASE.md` (if present) for details.
