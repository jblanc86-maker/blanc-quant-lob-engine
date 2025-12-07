# Observability Wiki Snapshot

This page condenses the workflow-usage observability playbook so you can drop it
into your internal wiki or adapt it for other BI stacks.

## 1. Data sources

| Source | Refresh cadence | Format | Notes |
|--------|-----------------|--------|-------|
| GitHub Action `Workflow Usage Report` | Weekly cron + manual | GitHub artifact (`reports/workflow-usage-report.json`, `reports/summary.md`) | Auto-discovers active workflows, tracks counts/success/duration |
| Datadog metrics (`ci.workflow.*`) | Same as workflow run | Time-series | Emitted only when the repo has `DATADOG_*` secrets configured |
| Local metrics export (`artifacts/metrics-export.json`) | On-demand via `scripts/verify_bench.py --run-metrics-exporter` | JSON | Useful for auto-tune experiments or ad-hoc dashboards |

## 2. How to publish metrics

1. Configure repository secrets:
   - `DATADOG_API_KEY`, `DATADOG_APP_KEY`, optional `DATADOG_SITE`.
2. Ensure `.github/workflows/workflow-usage-report.yml` stays enabled (already scheduled weekly at 07:00 UTC Monday).
3. Optional: other repos can reuse it via `workflow_call`:

```yaml
jobs:
  usage-report:
    uses: jblanc86-maker/blanc-quant-lob-engine/.github/workflows/workflow-usage-report.yml@main
    secrets:
      DATADOG_API_KEY: ${{ secrets.ORG_DATADOG_API_KEY }}
      DATADOG_APP_KEY: ${{ secrets.ORG_DATADOG_APP_KEY }}
```

4. Datadog will receive these metrics per workflow:
   - `ci.workflow.runs.total|success|failure|cancelled`
   - `ci.workflow.success_rate_pct`
   - `ci.workflow.avg_duration_ms`
   - Tags: `repo`, `workflow`, `file`

## 3. Suggested dashboards

| Widget | Metric/query | Purpose |
|--------|--------------|---------|
| Timeseries | `avg:ci.workflow.success_rate_pct{repo:<owner>/<repo>} by {workflow}` | Trend success rate and identify flaky workflows |
| Toplist | `sum:ci.workflow.runs.failure{*} by {workflow}` | Highlight workflows burning time |
| Heatmap | `avg:ci.workflow.avg_duration_ms{*} by {workflow}` | Spot slow jobs |
| Table (Looker/BI) | Columns from `workflows[]` array in the JSON artifact | Weekly report exported to leadership |

## 4. Alerting playbook (Datadog)

| Monitor | Query | Alert rule |
|---------|-------|------------|
| Success rate | `avg(last_30m):avg:ci.workflow.success_rate_pct{repo:<owner>/<repo>} by {workflow}` | Trigger when any workflow < 90% for 2 consecutive evaluations |
| Failure burst | `sum(last_1h):sum:ci.workflow.runs.failure{repo:<owner>/<repo>} by {workflow}` | Alert when failures >= 3/hr |
| Duration regression | `avg(last_1h):avg:ci.workflow.avg_duration_ms{workflow:CI}` | Warn when > 1.2 × trailing 7-day avg |

**Runbooks:** link monitors back to:
- `docs/workflow-usage-dashboard.md` (detailed ingest steps)
- `docs/local-checks.md` + `run_local_checks.sh` for repro guidance

## 5. Adapting to other BI tools

- **Looker**: Use the JSON schema (see `workflow-usage-dashboard.md`) and ingest into a derived table. Build Looks for success rate, failures, duration, and say a scoreboard per workflow.
- **Power BI / Tableau**: Schedule ingestion of `workflow-usage-report.json` via cloud storage or direct download. Use `windowDays` to slice by reporting windows.
- **Sheets / lightweight wiki**: Paste `reports/summary.md` directly; it already contains a Markdown table.

## 6. Next steps checklist

- [ ] Enable Datadog secrets in the repo (or org-level environment).
- [ ] Import the monitor queries above into your Datadog account; set owners + runbook links.
- [ ] Publish a wiki page pointing to this doc and `docs/workflow-usage-dashboard.md`.
- [ ] Wire Looker/BI ingestion to the weekly artifact if leadership needs recurring status.
- [ ] Encourage teams to run `scripts/verify_bench.py --run-metrics-exporter --auto-tune` when investigating regressions; the exported JSON can seed deeper analyses.
