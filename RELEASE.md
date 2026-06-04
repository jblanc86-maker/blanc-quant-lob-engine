# Blanc Quant LOB Engine v2.0.1 Release

Release published: 2026-06-04

## Overview

- Prebuilt binaries and supporting release artifacts for Blanc Quant LOB Engine v2.0.1.
- Public package showcases core order matching, market data replay, and telemetry.
- Suitable for students, hobbyists, and professionals evaluating market microstructure workflows.

## Release Information

This release includes the latest v2.0.1 binaries and supporting artifacts for
evaluation and reproducibility checks.

Note: the public package filename may use the **2.00 series branding** even when
the Git tag is `v2.0.1`.

## Publishing notes (CI)

- Release tags are **`v*`** (example: `v2.0.1`).
- Manual release runs (workflow dispatch) require a **`v`-prefixed** tag name.
- The release workflow syncs release metadata via GitHub's **versioned REST API**
  using `X-GitHub-Api-Version: 2026-03-10`.
- Legacy non-`v` tags (for example `2.0.0`, `2.0.1`) may still exist historically,
  but are **deprecated** and must not be treated as canonical releases.

The public repository contains the OSS replay and benchmarking harness source,
tests, scripts, and evidence artifacts for learning, experimentation, and
engineering evaluation. Enterprise BQL 2.0 packages production-focused
integrations, deployment support, and commercial terms.

## Why this release model

- Provides a reproducible public proof harness with deterministic evidence.
- Keeps production deployment integrations and support terms in the enterprise track.
- Gives teams a low-friction way to evaluate fit before pilot or commercial rollout.

## Enterprise Access

For production-focused integrations, support, and commercial terms, see
[`COMMERCIAL_LICENSE.md`](COMMERCIAL_LICENSE.md).

## Included Artifacts

- Prebuilt binaries
- Release manifests and packaging artifacts
- Public documentation needed to evaluate the engine
