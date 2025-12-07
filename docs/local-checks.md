# Local checks & PYTHONPATH helper

Running the Python guardrails locally often requires pointing `PYTHONPATH` at the
repo's helper modules (`scripts/`, `tools/`, future `bench/` helpers). Instead of
exporting the variable manually every time, use the new `run_local_checks.sh`
wrapper from the repo root.

```sh
./run_local_checks.sh          # verifies artifacts + renders report
./run_local_checks.sh python3 scripts/bench_report.py --help
```

The helper script:

1. Prepends `scripts/`, `tools/`, and `bench/` to `PYTHONPATH` (without duplicating entries).
2. Validates that `artifacts/bench.jsonl` and `artifacts/metrics.prom` exist before attempting checks.
3. Runs `scripts/verify_bench.py` followed by `scripts/bench_report.py` when no custom command is provided.
4. Re-uses the same environment for any custom command you pass after the script name.

This makes the tuned `PYTHONPATH` "permanent" for developer workflows without requiring shell-specific dotfiles.

## Metrics exporter + auto-tune flag

`scripts/verify_bench.py` gained two flags to help feed dashboards or auto-tuning pipelines:

- `--run-metrics-exporter`: writes a structured JSON bundle (default path
  `artifacts/metrics-export.json`) that merges the bench summary, Prometheus
  metrics, baseline stats, gate settings, and the final pass/fail verdict.
- `--auto-tune`: adds heuristic suggestions (`mad_multiplier`, `p99_multiplier`,
  and `p99_budget_ms`) to the export payload so you can see what multipliers
  would cover the most recent run with ~5% headroom.

Example:

```sh
python3 scripts/verify_bench.py \
  --profile standard \
  --run-metrics-exporter \
  --auto-tune
```

The exporter creates/updates `artifacts/metrics-export.json` and logs the path so
you can feed it to Datadog, Looker, or bespoke auto-tune tooling.
