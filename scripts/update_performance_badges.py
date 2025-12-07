#!/usr/bin/env python3
"""Generate Shields.io badge JSON from benchmark latency summaries."""

from __future__ import annotations

import argparse
import json
import math
import statistics
from pathlib import Path
from typing import Iterable, List, Optional


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Emit performance badge JSON based on bench.jsonl contents"
    )
    parser.add_argument(
        "--bench-file",
        default="artifacts/bench.jsonl",
        help="Path to bench.jsonl summary (default: artifacts/bench.jsonl)",
    )
    parser.add_argument(
        "--output-dir",
        default="badges",
        help="Directory where badge JSON files are written",
    )
    return parser.parse_args()


def _read_json_lines(path: Path) -> Iterable[dict]:
    for raw in path.read_text().splitlines():
        raw = raw.strip()
        if not raw:
            continue
        try:
            yield json.loads(raw)
        except json.JSONDecodeError:
            continue


def _percentile(values: List[float], quantile: float) -> float:
    if not values:
        raise ValueError("no latency samples available")
    if len(values) == 1:
        return values[0]
    # statistics.quantiles uses exclusive method by default; inclusive keeps endpoints.
    values.sort()
    pos = (len(values) - 1) * quantile
    lower = math.floor(pos)
    upper = math.ceil(pos)
    if lower == upper:
        return values[int(pos)]
    return values[lower] + (values[upper] - values[lower]) * (pos - lower)


def _load_summary(path: Path) -> tuple[float, float, float]:
    summary: Optional[dict] = None
    samples: List[float] = []
    for record in _read_json_lines(path):
        if record.get("summary"):
            summary = record
        elapsed = record.get("elapsed_ms")
        if isinstance(elapsed, (int, float)):
            samples.append(float(elapsed))
    if summary:
        p50 = float(summary.get("p50_ms", 0.0))
        p95 = float(summary.get("p95_ms", 0.0))
        p99 = float(summary.get("p99_ms", 0.0))
        return p50, p95, p99
    samples.sort()
    p50 = statistics.median(samples)
    p95 = _percentile(samples, 0.95)
    p99 = _percentile(samples, 0.99)
    return p50, p95, p99


def _pick_color(p99_ms: float) -> str:
    if p99_ms <= 500:
        return "green"
    if p99_ms <= 800:
        return "yellow"
    return "red"


def write_badges(directory: Path, p50: float, p95: float, p99: float) -> None:
    directory.mkdir(parents=True, exist_ok=True)
    color = _pick_color(p99)
    badge_perf = {
        "schemaVersion": 1,
        "label": "p99 ms",
        "message": f"{p99:.2f}",
        "color": color,
        "namedLogo": "github",
    }
    badge_extra = {
        "schemaVersion": 1,
        "label": "p50/p95/p99",
        "message": f"{p50:.2f}/{p95:.2f}/{p99:.2f} ms",
        "color": color,
    }
    (directory / "performance.json").write_text(json.dumps(badge_perf, indent=2) + "\n", encoding="utf-8")
    (directory / "performance_extra.json").write_text(
        json.dumps(badge_extra, indent=2) + "\n", encoding="utf-8"
    )


def main() -> int:
    args = parse_args()
    bench_path = Path(args.bench_file)
    if not bench_path.exists():
        raise SystemExit(f"bench file {bench_path} does not exist")
    p50, p95, p99 = _load_summary(bench_path)
    write_badges(Path(args.output_dir), p50, p95, p99)
    print(f"Generated badges from {bench_path}: p50={p50:.2f}, p95={p95:.2f}, p99={p99:.2f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
