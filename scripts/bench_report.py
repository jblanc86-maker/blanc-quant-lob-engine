#!/usr/bin/env python3
"""HTML analytics report generator for benchmark results.

This script reads benchmark data from bench.jsonl and metrics.prom,
then generates an HTML dashboard with latency visualizations and
digest verification status.

Usage:
    python scripts/bench_report.py [--bench-file FILE] [--metrics-file FILE] [--output-dir DIR]

Output:
    artifacts/report/index.html - Interactive HTML dashboard

To enable visitor tracking in the report, add a Google Analytics snippet.
See README.md for configuration details.
"""

from __future__ import annotations

import argparse
import html
import json
import os
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List, Optional, Sequence


@dataclass
class BenchRun:
    """Single benchmark run data."""

    run: int
    elapsed_ms: float
    line: str


@dataclass
class BenchSummary:
    """Summary statistics from benchmark runs."""

    runs: int
    p50_ms: float
    p95_ms: float
    p99_ms: float


@dataclass
class ReportData:
    """Aggregated data for report generation."""

    bench_runs: List[BenchRun]
    summary: Optional[BenchSummary]
    metrics: Dict[str, float]
    generated_at: str


def parse_bench_jsonl(path: Path) -> tuple[List[BenchRun], Optional[BenchSummary]]:
    """Parse benchmark JSONL file into runs and summary."""
    runs: List[BenchRun] = []
    summary: Optional[BenchSummary] = None

    if not path.exists():
        return runs, summary

    for raw in path.read_text().splitlines():
        line = raw.strip()
        if not line:
            continue
        try:
            data = json.loads(line)
        except json.JSONDecodeError:
            continue

        if data.get("summary"):
            summary = BenchSummary(
                runs=int(data.get("runs", 0)),
                p50_ms=float(data.get("p50_ms", 0)),
                p95_ms=float(data.get("p95_ms", 0)),
                p99_ms=float(data.get("p99_ms", 0)),
            )
        elif "run" in data:
            runs.append(
                BenchRun(
                    run=int(data.get("run", 0)),
                    elapsed_ms=float(data.get("elapsed_ms", 0)),
                    line=str(data.get("line", "")),
                )
            )

    return runs, summary


def parse_prom_metrics(path: Path) -> Dict[str, float]:
    """Parse Prometheus textfile metrics."""
    metrics: Dict[str, float] = {}

    if not path.exists():
        return metrics

    for raw in path.read_text().splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split(None, 1)
        if len(parts) != 2:
            continue
        key, value = parts
        try:
            metrics[key] = float(value)
        except ValueError:
            continue

    return metrics


def generate_html_report(data: ReportData, ga_id: Optional[str] = None) -> str:
    """Generate HTML report from benchmark data."""
    # Build latency chart data
    chart_labels = [str(run.run) for run in data.bench_runs]
    chart_values = [run.elapsed_ms for run in data.bench_runs]

    # Summary stats
    summary_html = ""
    if data.summary:
        summary_html = f"""
        <div class="stats-grid">
            <div class="stat-card">
                <div class="stat-value">{data.summary.runs}</div>
                <div class="stat-label">Total Runs</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{data.summary.p50_ms:.3f} ms</div>
                <div class="stat-label">P50 Latency</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{data.summary.p95_ms:.3f} ms</div>
                <div class="stat-label">P95 Latency</div>
            </div>
            <div class="stat-card">
                <div class="stat-value">{data.summary.p99_ms:.3f} ms</div>
                <div class="stat-label">P99 Latency</div>
            </div>
        </div>
        """

    # Metrics table
    metrics_rows = ""
    for key, value in sorted(data.metrics.items()):
        escaped_key = html.escape(key)
        metrics_rows += f"<tr><td>{escaped_key}</td><td>{value:.4f}</td></tr>\n"

    # Google Analytics snippet
    ga_snippet = ""
    if ga_id:
        escaped_ga_id = html.escape(ga_id)
        ga_snippet = f"""
    <!-- Google Analytics -->
    <script async src="https://www.googletagmanager.com/gtag/js?id={escaped_ga_id}"></script>
    <script>
      window.dataLayer = window.dataLayer || [];
      function gtag() {{ dataLayer.push(arguments); }}
      gtag('js', new Date());
      gtag('config', '{escaped_ga_id}');
    </script>
    """

    escaped_generated_at = html.escape(data.generated_at)
    chart_labels_json = json.dumps(chart_labels)
    chart_values_json = json.dumps(chart_values)

    return f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Blanc Quant LOB Engine - Benchmark Report</title>
    {ga_snippet}
    <style>
        :root {{
            --bg-primary: #1a1a2e;
            --bg-secondary: #16213e;
            --text-primary: #eee;
            --text-secondary: #aaa;
            --accent: #0f3460;
            --highlight: #e94560;
        }}
        * {{ box-sizing: border-box; margin: 0; padding: 0; }}
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: var(--bg-primary);
            color: var(--text-primary);
            line-height: 1.6;
            padding: 2rem;
        }}
        .container {{ max-width: 1200px; margin: 0 auto; }}
        header {{
            text-align: center;
            margin-bottom: 2rem;
            padding-bottom: 1rem;
            border-bottom: 2px solid var(--accent);
        }}
        h1 {{ font-size: 2rem; margin-bottom: 0.5rem; }}
        .timestamp {{ color: var(--text-secondary); font-size: 0.9rem; }}
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 1rem;
            margin-bottom: 2rem;
        }}
        .stat-card {{
            background: var(--bg-secondary);
            padding: 1.5rem;
            border-radius: 8px;
            text-align: center;
        }}
        .stat-value {{
            font-size: 1.8rem;
            font-weight: bold;
            color: var(--highlight);
        }}
        .stat-label {{
            color: var(--text-secondary);
            font-size: 0.9rem;
            margin-top: 0.5rem;
        }}
        .section {{
            background: var(--bg-secondary);
            padding: 1.5rem;
            border-radius: 8px;
            margin-bottom: 1.5rem;
        }}
        .section h2 {{
            font-size: 1.2rem;
            margin-bottom: 1rem;
            color: var(--highlight);
        }}
        .chart-container {{
            position: relative;
            height: 300px;
            width: 100%;
        }}
        table {{
            width: 100%;
            border-collapse: collapse;
        }}
        th, td {{
            padding: 0.75rem;
            text-align: left;
            border-bottom: 1px solid var(--accent);
        }}
        th {{ color: var(--highlight); }}
        .visitor-badge {{
            text-align: center;
            margin-top: 2rem;
        }}
        footer {{
            text-align: center;
            margin-top: 2rem;
            padding-top: 1rem;
            border-top: 1px solid var(--accent);
            color: var(--text-secondary);
            font-size: 0.85rem;
        }}
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div class="container">
        <header>
            <h1>Blanc Quant LOB Engine</h1>
            <p>Benchmark Analytics Report</p>
            <p class="timestamp">Generated: {escaped_generated_at}</p>
        </header>

        {summary_html}

        <div class="section">
            <h2>Latency Distribution</h2>
            <div class="chart-container">
                <canvas id="latencyChart"></canvas>
            </div>
        </div>

        <div class="section">
            <h2>Prometheus Metrics</h2>
            <table>
                <thead>
                    <tr><th>Metric</th><th>Value</th></tr>
                </thead>
                <tbody>
                    {metrics_rows}
                </tbody>
            </table>
        </div>

        <div class="visitor-badge">
            <img src="https://visitor-badge.laobi.icu/badge?page_id=blanc86-maker.blanc-quant-lob-engine.report" alt="Visitor Badge">
        </div>

        <footer>
            <p>Blanc Quant LOB Engine &copy; 2024</p>
            <p>Deterministic C++20 limit order book replay engine</p>
        </footer>
    </div>

    <script>
        const ctx = document.getElementById('latencyChart').getContext('2d');
        new Chart(ctx, {{
            type: 'line',
            data: {{
                labels: {chart_labels_json},
                datasets: [{{
                    label: 'Elapsed Time (ms)',
                    data: {chart_values_json},
                    borderColor: '#e94560',
                    backgroundColor: 'rgba(233, 69, 96, 0.1)',
                    fill: true,
                    tension: 0.3
                }}]
            }},
            options: {{
                responsive: true,
                maintainAspectRatio: false,
                plugins: {{
                    legend: {{
                        labels: {{ color: '#eee' }}
                    }}
                }},
                scales: {{
                    x: {{
                        title: {{ display: true, text: 'Run #', color: '#aaa' }},
                        ticks: {{ color: '#aaa' }},
                        grid: {{ color: 'rgba(255,255,255,0.1)' }}
                    }},
                    y: {{
                        title: {{ display: true, text: 'Latency (ms)', color: '#aaa' }},
                        ticks: {{ color: '#aaa' }},
                        grid: {{ color: 'rgba(255,255,255,0.1)' }}
                    }}
                }}
            }}
        }});
    </script>
</body>
</html>
"""


def build_arg_parser() -> argparse.ArgumentParser:
    """Build command-line argument parser."""
    parser = argparse.ArgumentParser(
        description="Generate HTML analytics report from benchmark data"
    )
    parser.add_argument(
        "--bench-file",
        default="artifacts/bench.jsonl",
        help="Path to benchmark JSONL file (default: artifacts/bench.jsonl)",
    )
    parser.add_argument(
        "--metrics-file",
        default="artifacts/metrics.prom",
        help="Path to Prometheus metrics file (default: artifacts/metrics.prom)",
    )
    parser.add_argument(
        "--output-dir",
        default="artifacts/report",
        help="Output directory for HTML report (default: artifacts/report)",
    )
    parser.add_argument(
        "--ga-id",
        default=os.getenv("GOOGLE_ANALYTICS_ID"),
        help="Google Analytics measurement ID (e.g., G-XXXXXXXXXX)",
    )
    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    """Main entry point."""
    parser = build_arg_parser()
    args = parser.parse_args(argv)

    bench_path = Path(args.bench_file)
    metrics_path = Path(args.metrics_file)
    output_dir = Path(args.output_dir)

    # Parse input files
    bench_runs, summary = parse_bench_jsonl(bench_path)
    metrics = parse_prom_metrics(metrics_path)

    # Build report data
    report_data = ReportData(
        bench_runs=bench_runs,
        summary=summary,
        metrics=metrics,
        generated_at=datetime.now(timezone.utc).isoformat(),
    )

    # Generate HTML
    html_content = generate_html_report(report_data, ga_id=args.ga_id)

    # Write output
    output_dir.mkdir(parents=True, exist_ok=True)
    output_file = output_dir / "index.html"
    output_file.write_text(html_content)

    print(f"Report generated: {output_file}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
