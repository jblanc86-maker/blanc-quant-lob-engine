# BQS Enterprise (Private Add-On)

This directory is the **enterprise extension layer** for Blanc Quant LOB Engine.

## Goals

- Keep the public OSS build **fully functional** without enterprise code.
- Provide **clean extension points** for production feed/session adapters and operational hardening.
- Allow enterprise builds to be enabled explicitly and reliably.

## How it’s enabled

Enterprise components are built only when CMake is configured with:

- `-DENABLE_BQS_ENTERPRISE=ON`

When disabled (default), the OSS targets, tests, and benchmark harness behave exactly as before.

## Public vs Enterprise boundary

**OSS (public repo)** focuses on:

- Deterministic replay + digest proof
- CI latency regression gates
- Evidence artifact generation

**Enterprise (this directory)** focuses on:

- Real feed and order-flow adapters (ITCH/FIX/venue-specific)
- Session management, recovery, gap handling
- Deploy templates, ops defaults, and production integration surfaces

## Stability contract (what enterprise code must not do)

- Must not change default OSS determinism or benchmark results when enterprise is disabled.
- Must not require secrets at build time.
- Must avoid embedding proprietary venue specs/content in this public tree.

## Code layout

- `include/bqs/adapters/` — stable adapter interfaces (placeholders)
- `src/` — enterprise library implementation stubs
- `tests/` — enterprise-only unit tests
- `deploy/` — minimal deployment templates (systemd/k8s)
