# VORTEX Scoring Notes

VORTEX blends multiple signal families into one ranked score. The defaults mirror the README summary but this document adds tuning context for boosts, decays, and work-in-progress guards.

## Base Weights

| Signal | Weight | Description |
| --- | --- | --- |
| Fit | 0.35 | Resume/company alignment, skill overlap, mission affinity |
| Comp | 0.25 | Compensation reachability plus equity band projections |
| Timing | 0.20 | Role freshness, recruiter responsiveness, pipeline momentum |
| Warm | 0.15 | Mutual connections, referral potential, exec sponsorship |
| Brand | 0.05 | Employer signal (perceived stability, brand pull) |

Formula:

$$overall = 0.35\cdot fit + 0.25\cdot comp + 0.20\cdot timing + 0.15\cdot warm + 0.05\cdot brand$$

Baseline weights live in `vortex/weights.yaml`. Override per-profile by dropping an alternate YAML into `profiles/<name>/weights.yaml`.

## Boosts

Boosts add uplift when tactical signals are detected:

- **Headcount Spike**: +5 to timing when requisition count increases week-over-week.
- **Mutual Warmth**: +7 to warm when there is a 2nd-degree connection willing to intro.
- **In-flight Referral**: +10 distributed across warm/fit once a referral is logged.
- **Strategic Initiative**: +4 to brand when the role maps to a tracked initiative.

## Decays

Decays prevent stale opportunities from clogging the queue:

- **Age > 21 days**: subtract 8 from timing and 3 from overall score.
- **No Replies in 7 days**: subtract 5 from warm.
- **Recruiter Turnover**: subtract 4 from brand when contacts churn.
- **Low Engagement Loop**: subtract 6 from fit if two cadences fail with no signal.

## Guards & WIP Caps

Guards ensure CommandPost never overloads operators:

- **Channel WIP**: max 12 concurrent outreach threads per channel (`SG_PROFILE` tuned).
- **Per-Persona Cap**: limit 6 active opportunities per persona to keep cadences authentic.
- **Compliance Guard**: block `sg strike` if legal/finance toggles request a hold.
- **Blackout Windows**: optional `profiles/*.yaml` windows to pause campaigns (e.g., holidays).

## Implementation Notes

- Store boosts/decays as structured events so SITREP can explain score deltas.
- Keep deterministic fixtures in `tests/vortex/` to assert the final score for canonical opportunities.
- Emit `actions.csv` rows with `score_before`, `score_after`, and `why` to make CommandPost auditable.
