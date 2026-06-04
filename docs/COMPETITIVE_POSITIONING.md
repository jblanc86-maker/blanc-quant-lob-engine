# Competitive Positioning

## Purpose

This document clarifies where Blanc Quant LOB Engine (BQL) fits relative to adjacent vendor categories and common internal alternatives.

BQL is best framed as an **engine-level deterministic replay and latency-proof infrastructure layer**.

## Segment-by-segment positioning

| Segment                           | Example vendors                                                    | What they usually optimize                                                  | Where BQL fits                                                                                                                    |
| --------------------------------- | ------------------------------------------------------------------ | --------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| Network latency / packet evidence | Corvil (Pico), Beeks Analytics, Velocimetrics, LiveAction, cPacket | Packet capture, wire latency visibility, network forensic workflows         | **Complementary**. BQL proves deterministic replay and CI-gated engine-level latency behavior tied to code and release artifacts. |
| Exchange / venue analytics        | ICE Connectivity, CME Smart Stream, Nasdaq analytics tooling       | Venue-side feed/connectivity insight and market data monitoring             | **Adjacent**. BQL validates internal replay determinism and release regression controls after venue data is ingested.             |
| TCA / execution quality analytics | big xyt, BMLL, Tradefeedr, ISS LiquidMetrix                        | Post-trade execution quality, benchmark attribution, venue/broker analytics | **Different buyer/job**. BQL addresses infra-latency determinism evidence and pre-production release confidence.                  |
| In-house internal build           | Tier-1 banks / HFT internal teams                                  | Full custom ownership, internal integration depth                           | **Primary alternative**. BQL competes on time-to-proof, reproducibility, and lower long-term maintenance burden.                  |

## Core differentiators to lead with

1. **Deterministic replay proof loop**
   Same input + same gate decisions → same digest.
2. **CI-enforced latency contracts**
   Release gates on p50/p95/p99 (and tail metrics where configured).
3. **Audit-ready evidence artifacts**
   Structured outputs (`bench.jsonl`, `metrics.prom`, manifests) preserved with runs.
4. **Canonical verification**
   Order/shard-invariant aggregate digest capabilities for reproducibility claims.

## Objection handling

### "We already have network latency tooling"

Response: Keep network tooling for packet-level visibility; use BQL for deterministic engine replay validation and release gating.

### "We already run TCA"

Response: TCA answers execution quality outcomes. BQL verifies deterministic infra behavior and regression risk before deployment.

### "We can build this ourselves"

Response: In-house build is valid but expensive over time. BQL shortens time-to-proof, standardizes evidence generation, and reduces maintenance burden.

## Best-fit buyer profiles

- Platform / market infrastructure engineering teams
- Release governance and production reliability owners
- Teams that must demonstrate reproducible latency and determinism evidence

## Messaging guardrails

- Do **not** position BQL as a packet-capture replacement.
- Do **not** position BQL as a full standalone TCA platform.
- Position BQL as the deterministic proof and release-control layer within an existing observability/trading stack.
