# Workflow Usage Dashboard

This repository now emits a structured `reports/workflow-usage-report.json` artifact every time the `Workflow Usage Report` workflow runs. The artifact captures per-workflow execution counts, success rates, cancellation totals, and average durations over the last _N_ days (`WINDOW_DAYS`, default `7`).

This guide shows how to turn that JSON into an external dashboard, focusing on Datadog (automated) and Looker (manual ingest).

## Datadog metrics pipeline (automated)

The main workflow (`.github/workflows/workflow-usage-report.yml`) includes an optional "Push metrics to Datadog" step. When the required secrets are available, the job publishes six metrics per workflow:

| Metric name | Description | Units |
|-------------|-------------|-------|
| `ci.workflow.runs.total` | Number of runs observed in the reporting window | count |
| `ci.workflow.runs.success` | Successful runs | count |
| `ci.workflow.runs.failure` | Failed runs | count |
| `ci.workflow.runs.cancelled` | Cancelled runs | count |
| `ci.workflow.success_rate_pct` | Percentage of successful runs | percent |
| `ci.workflow.avg_duration_ms` | Average runtime | milliseconds |

Each metric is tagged with `repo:<owner>/<repo>`, `workflow:<display name>`, and `file:<path>`, making it easy to group or filter in Datadog.

### Setup

1. In **Repository Settings → Secrets and variables → Actions**, add the following repository secrets (or define them on a reusable workflow that calls this one):
   - `DATADOG_API_KEY`: Datadog API key with Metrics ingestion permissions.
   - `DATADOG_APP_KEY`: Datadog application key allowed to submit metrics.
   - `DATADOG_SITE` (optional): Datadog site suffix, e.g. `datadoghq.eu`. Defaults to `datadoghq.com` when unset.
2. The workflow already declares these secrets under the `workflow_call` trigger, which keeps the schema checker happy and allows other workflows to call it.
3. Once the secrets are present, the step automatically publishes metrics on every scheduled or manually triggered run. Missing secrets simply result in a friendly skip notice.

### Dashboarding ideas

- **Overall health**: Query `ci.workflow.success_rate_pct` grouped by `workflow` to highlight flakiness trends.
- **Hotspot detection**: Configure a toplist widget for `ci.workflow.runs.failure` with a 7-day rollup to surface the noisiest workflows.
- **Latency tracking**: Plot `ci.workflow.avg_duration_ms` with a `rollup(avg, 1d)` to watch for slowdowns.
- **Composite SLO**: Use a formula / query value widget combining `sum:ci.workflow.runs.success` and `sum:ci.workflow.runs.total` to get overall success coverage.

## Looker or ad-hoc analysis (manual)

If your analytics platform ingests files (e.g., Looker, Sheets, BigQuery), you can reuse the same JSON artifact.

1. Download the latest `workflow-usage-report` artifact from the workflow run summary.
2. Parse the JSON (`reports/workflow-usage-report.json`) and load it into your warehouse. The top-level schema looks like:

```json
{
  "generatedAt": "2025-12-06T07:00:00.000Z",
  "windowDays": 7,
  "workflows": [
    {
      "workflow": "CI",
      "file": ".github/workflows/ci.yml",
      "count": 42,
      "success": 39,
      "failure": 2,
      "cancelled": 1,
      "inProgress": 0,
      "successRate": 92.9,
      "avgDurationMs": 315000
    }
  ]
}
```
3. In Looker, create an ETL that ingests the JSON into a table with the columns above plus `generated_at` and `window_days`. From there you can build Looks around reliability KPIs (success %, MTTR proxies, etc.).

### Automating Looker / Power BI ingestion

Leadership dashboards usually want fresh data without manual downloads. The
artifact lives behind the GitHub Actions API, so set up a tiny sync job and aim
your BI tool at the staged files.

1. **Schedule an artifact sync** (GitHub → object storage):
   ```bash
   #!/usr/bin/env bash
   set -euo pipefail
   gh run download --repo jblanc86-maker/blanc-quant-lob-engine \
     --name workflow-usage-report --pattern 'reports/*.json' --dir /tmp/usage-report
   ts=$(date +%Y%m%d-%H%M)
   aws s3 cp /tmp/usage-report/reports/workflow-usage-report.json \
     s3://observability-lake/workflow-usage/${ts}.json
   ```
   - Run via GitHub Actions cron, Airflow, or any scheduler shortly after the
     weekly workflow (07:00 UTC Monday) to minimize lag.
   - If you prefer Azure/GCP, swap the `aws s3` copy for `az storage` or
     `gcloud storage`.
2. **Looker**:
   - Point a derived table at the S3/GCS bucket. Example SQL (Presto/Athena):
     ```sql
     SELECT json_extract_scalar(t.json, '$.generatedAt') AS generated_at,
            json_extract_scalar(w, '$.workflow')        AS workflow,
            CAST(json_extract_scalar(w, '$.count') AS BIGINT) AS runs,
            CAST(json_extract_scalar(w, '$.successRate') AS DOUBLE) AS success_rate,
            CAST(json_extract_scalar(w, '$.avgDurationMs') AS DOUBLE) AS avg_duration_ms
     FROM usage_reports t
     CROSS JOIN UNNEST(cast(json_extract(t.json, '$.workflows') AS array(json))) w;
     ```
   - Reuse the visualization ideas from the Datadog section (success-rate trend,
     failure toplist, duration heatmap) with Looks or Dashboards.
3. **Power BI**:
   - Create a `Power Query` dataflow using the REST endpoint that serves the
     staged JSON (e.g., Azure Blob public SAS URL) or call the GitHub API
     directly with an access token header.
   - Use `Json.Document(Web.Contents(...))` to expand the `workflows` array and
     build visuals mirroring the Datadog/Looker widgets (gauge for success rate,
     clustered bar for failures, line chart for duration).
   - Schedule refresh (daily or weekly) to line up with the artifact sync.

> Tip: keep a lightweight Markdown summary (`reports/summary.md`) as a backup
> data source—Power BI can ingest it as a table if JSON ingestion fails.

## Reusing the workflow via `workflow_call`

Because the workflow now exposes a `workflow_call` interface, other repositories can import this reporting job and automatically emit the same metrics/artifacts. Example:

```yaml
jobs:
  usage-report:
    uses: jblanc86-maker/blanc-quant-lob-engine/.github/workflows/workflow-usage-report.yml@main
    secrets:
      DATADOG_API_KEY: ${{ secrets.ORG_DATADOG_API_KEY }}
      DATADOG_APP_KEY: ${{ secrets.ORG_DATADOG_APP_KEY }}
      DATADOG_SITE: datadoghq.com
```

This makes it easy to centralize observability without duplicating logic.

## Datadog monitors & alerting

Once the metrics land in Datadog, wire up a few low-noise monitors to make the
feedback loop actionable:

| Use case | Suggested query | Threshold |
|----------|-----------------|-----------|
| Success rate watchdog | `avg(last_15m):avg:ci.workflow.success_rate_pct{repo:<owner>/<repo>} < 90` | Warn: 92, Alert: 90 |
| Failing workflow hotspot | `sum(last_1h):sum:ci.workflow.runs.failure{workflow:*} by {workflow}` | Alert when any workflow > 3 failures/hour |
| Duration regression | `avg(last_1h):avg:ci.workflow.avg_duration_ms{workflow:CI}` | Alert when > historical p95 (e.g., 600000 ms) |

Implementation tips:

1. Add runbook links back to this repository (e.g., `docs/workflow-usage-dashboard.md`).
2. Tag monitors with `team:platform` (or similar) so ownership stays obvious.
3. Use composite monitors (`A AND NOT B`) to suppress noise during scheduled maintenance windows.

## Operational tips

- Tune `WINDOW_DAYS` per repository by setting a job-level environment override if you want smaller or larger windows.
- If Datadog ingestion ever fails, the workflow logs include the HTTP status and body for easier debugging.
- When using Looker, schedule the ingestion shortly after the GitHub workflow's cron to keep dashboards up to date.
- For Power BI/Tableau, treat `reports/workflow-usage-report.json` as a scheduled import and build visuals mirroring the Datadog widgets above (success rate gauge, failure toplist, duration heatmap).
