# Contributing to Blanc LOB Engine

Thanks for your interest in contributing! This guide outlines our development workflow, coding standards, and how to propose improvements safely.

## TL;DR

* Fork or create a feature branch from `main`.
* Install tooling and run pre-commit locally: `pip install pre-commit && pre-commit install && pre-commit run -a`.
* Build & run smoke tests: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j && scripts/verify_golden.sh`.
* Open a PR with a concise title and a clear description of the change and its rationale.

## Development Environment

* CMake, Ninja, and a C++20 compiler (Clang/GCC).
* Python 3.11+ for scripts and pre-commit hooks.
* Optional: Docker (see `Dockerfile`) for hermetic builds.

## Pre-commit Hooks

We use pre-commit to keep the codebase consistent and secure:

* whitespace & EOF normalization
* YAML/JSON/TOML validation
* (optionally) clang-format, codespell, detect-secrets

Run locally before committing:

```sh
pip install pre-commit
pre-commit install
pre-commit run --all-files
```

## Build & Test

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
scripts/verify_golden.sh
```

For Debug builds with sanitizers:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build -j
```

## Commit Messages

* Use conventional style where possible, e.g. `feat:`, `fix:`, `docs:`, `ci:`, `chore:`.
* Keep the first line under ~72 chars; wrap additional detail in the body.

## Pull Requests

* Keep PRs focused and small when possible.
* Include motivation, approach, and testing notes.
* Link related issues.
* CI must pass before review/merge.

## Security

* Never commit secrets. A detect-secrets workflow runs in CI.
* Report vulnerabilities privately via the Security Policy.

## License

* By contributing, you agree your contributions are licensed under this repository’s license.

---

Thanks again for helping improve Blanc LOB Engine!
