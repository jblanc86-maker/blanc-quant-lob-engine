#!/usr/bin/env python3
"""Tail-latency and determinism guardrail checker.

The script is intentionally tiny so it can run inside CI workflows or locally
before cutting a release. It reads:

* `bench.jsonl` (first line of NDJSON summarising the latest run)
* `metrics.prom` (for schema validation)
* `artifacts/baselines/*.json` (per-runner golden stats)

It then enforces profile-specific gates (strict/standard/lenient) with optional
overrides via environment variables.
"""

from __future__ import annotations

import argparse
import json
import os
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, Optional, Sequence

PROFILE_PRESETS = {
    "strict": {
        "mad_multiplier": 2.0,
        "p99_multiplier": 1.05,
        "epsilon_ms": 0.03,
        "require_digest": True,
        "description": "Nightly/CI goldens",
    },
    "standard": {
        "mad_multiplier": 3.0,
        "p99_multiplier": 1.10,
        "epsilon_ms": 0.05,
        "require_digest": False,
        "description": "PR gate",
    },
    "lenient": {
        "mad_multiplier": 4.0,
        "p99_multiplier": 1.20,
        "epsilon_ms": 0.07,
        "require_digest": False,
        "description": "Exploratory",
    },
}

REQUIRED_METRICS = {
    "lob_p50_ms",
    "lob_p95_ms",
    "lob_p99_ms",
    "lob_p999_ms",    # p99.9  — required for DSEF Tail Latency Purity
    "lob_p9999_ms",   # p99.99 — required for DSEF Tail Latency Purity
    "lob_publish_allowed",
}


@dataclass
class BenchResult:
    p99_ms: float
    p999_ms: float = 0.0   # p99.9
    p9999_ms: float = 0.0  # p99.99
    digest: Optional[str] = None
    determinism: Optional[bool] = None


def _optional_float(value: Any) -> Optional[float]:
    if value is None:
        return None
    try:
        return float(value)
    except (TypeError, ValueError):
        return None


def auto_tune_gate(bench: BenchResult, baseline: Dict[str, Any], allowed_ms: float) -> Dict[str, Any]:
    median = _optional_float(baseline.get("median_p99_ms")) or bench.p99_ms
    mad = _optional_float(baseline.get("mad_p99_ms"))
    last = _optional_float(baseline.get("last_p99_ms")) or median or bench.p99_ms

    suggestions: Dict[str, Any] = {
        "p99_budget_ms": round(bench.p99_ms * 1.05, 4),
        "current_allowed_ms": round(allowed_ms, 4),
        "notes": "Adds ~5% headroom using the latest measurement.",
    }
    if mad and mad > 0:
        suggestions["mad_multiplier"] = round(max(1.0, (bench.p99_ms - median) / mad + 0.5), 2)
    if last and last > 0:
        suggestions["p99_multiplier"] = round(max(1.0, bench.p99_ms / last + 0.02), 3)
    return suggestions


def export_metrics_bundle(
    output_path: Path,
    bench: BenchResult,
    metrics: Dict[str, float],
    baseline: Dict[str, Any],
    allowed_ms: float,
    status: str,
    runner_id: str,
    profile_name: str,
    profile_cfg: Dict[str, Any],
    include_auto_tune: bool,
) -> None:
    payload: Dict[str, Any] = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "status": status,
        "runner": runner_id,
        "profile": profile_name,
        "gates": {
            "allowed_p99_ms": round(allowed_ms, 4),
            "p99_multiplier": profile_cfg["p99_multiplier"],
            "mad_multiplier": profile_cfg["mad_multiplier"],
            "epsilon_ms": profile_cfg["epsilon_ms"],
        },
        "bench": {
            "p99_ms": bench.p99_ms,
            "p999_ms": bench.p999_ms,
            "p9999_ms": bench.p9999_ms,
            "digest": bench.digest,
            "determinism": bench.determinism,
        },
        "metrics": metrics,
        "baseline": baseline,
    }
    if include_auto_tune:
        payload["auto_tune"] = auto_tune_gate(bench, baseline, allowed_ms)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    print(f"[metrics-exporter] wrote {output_path}")


def _read_first_json_line(path: Path) -> Dict[str, Any]:
    for raw in path.read_text().splitlines():
        line = raw.strip()
        if not line:
            continue
        try:
            return json.loads(line)
        except json.JSONDecodeError as exc:
            raise SystemExit(f"✖ bench file {path} contains invalid JSON: {exc}")
    raise SystemExit(f"✖ bench file {path} is empty")


def parse_bench(path: Path) -> BenchResult:
    payload = _read_first_json_line(path)

    def _require_float(key: str) -> float:
        value = payload.get(key)
        if value is None:
            raise SystemExit(f"✖ bench payload missing '{key}'")
        try:
            return float(value)
        except (TypeError, ValueError) as exc:
            raise SystemExit(f"✖ bench '{key}' not a float: {exc}")

    digest = payload.get("digest_fnv") or payload.get("actual")
    return BenchResult(
        p99_ms=_require_float("p99_ms"),
        p999_ms=float(payload.get("p999_ms") or 0.0),
        p9999_ms=float(payload.get("p9999_ms") or 0.0),
        digest=digest,
        determinism=payload.get("determinism"),
    )


def parse_prom_metrics(path: Path) -> Dict[str, float]:
    metrics: Dict[str, float] = {}
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
        except ValueError as exc:
            raise SystemExit(f"✖ metric '{key}' expected numeric value, got '{value}': {exc}")
    missing = REQUIRED_METRICS - metrics.keys()
    if missing:
        raise SystemExit(f"✖ metrics file {path} missing keys: {', '.join(sorted(missing))}")
    return metrics


def load_baseline(path: Path, runner_id: str) -> Dict[str, Any]:
    if not path.exists():
        raise SystemExit(f"✖ baseline file {path} not found. Run golden calibration first.")
    try:
        data = json.loads(path.read_text())
    except json.JSONDecodeError as exc:
        raise SystemExit(f"✖ baseline file {path} is not valid JSON: {exc}")
    stats = data.get("runners", {}).get(runner_id) or data.get("defaults")
    if stats is None:
        raise SystemExit(f"✖ no baseline entry for runner '{runner_id}' and no defaults fallback")
    for key in ("median_p99_ms", "mad_p99_ms"):
        if key not in stats:
            raise SystemExit(f"✖ baseline entry missing '{key}'")
    return stats


def compute_allowed(
    baseline: Dict[str, Any],
    mad_multiplier: float,
    p99_multiplier: float,
    epsilon_ms: float,
    budget_cap_ms: Optional[float],
) -> float:
    candidates = []
    median = baseline.get("median_p99_ms")
    mad = baseline.get("mad_p99_ms")
    last = baseline.get("last_p99_ms") or baseline.get("p99_ms") or median
    if median is not None and mad is not None:
        candidates.append(float(median) + mad_multiplier * float(mad) + epsilon_ms)
    if last is not None:
        candidates.append(float(last) * p99_multiplier + epsilon_ms)
    if budget_cap_ms is not None:
        candidates.append(budget_cap_ms)
    if not candidates:
        raise SystemExit("✖ unable to compute allowance; baseline missing usable stats")
    return min(candidates)


def resolve_runner_id(explicit: Optional[str]) -> str:
    if explicit:
        return explicit
    name = os.getenv("RUNNER_NAME")
    arch = os.getenv("RUNNER_ARCH")
    os_name = os.getenv("RUNNER_OS")
    pieces = [piece for piece in (name, os_name, arch) if piece]
    return "-".join(pieces) or "manual-runner"


def env_float(key: str) -> Optional[float]:
    raw = os.getenv(key)
    if raw in (None, "", "None"):
        return None
    try:
        return float(raw)
    except ValueError as exc:
        raise SystemExit(f"✖ environment {key} must be float-compatible: {exc}")


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Tail-latency & digest guardrail checker")
    parser.add_argument("--bench-file", default="artifacts/bench.jsonl", help="NDJSON benchmark summary")
    parser.add_argument("--metrics-file", default="artifacts/metrics.prom", help="Prometheus textfile output")
    parser.add_argument(
        "--baseline-file",
        default="artifacts/baselines/sample_runner.json",
        help="Per-runner baseline JSON",
    )
    parser.add_argument("--profile", choices=PROFILE_PRESETS.keys(), default=os.getenv("PROFILE", "standard"))
    parser.add_argument("--runner-id", default=None, help="Override runner identifier")
    parser.add_argument("--p99-multiplier", type=float, default=None, help="Override p99 multiplier")
    parser.add_argument("--mad-multiplier", type=float, default=None, help="Override MAD multiplier")
    parser.add_argument("--epsilon-ms", type=float, default=None, help="Override epsilon slack")
    parser.add_argument("--p99-budget-ms", type=float, default=None, help="Absolute cap (optional)")
    parser.add_argument("--require-digest", action="store_true", help="Force digest equality regardless of profile")
    parser.add_argument(
        "--run-metrics-exporter",
        action="store_true",
        help="Write a JSON bundle with bench, baseline, and Prometheus metrics",
    )
    parser.add_argument(
        "--metrics-export-path",
        default="artifacts/metrics-export.json",
        help="Path to write the metrics export bundle",
    )
    parser.add_argument(
        "--auto-tune",
        action="store_true",
        help="Include auto-tuned gate suggestions in the export payload",
    )
    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = build_arg_parser()
    args = parser.parse_args(argv)
    if args.auto_tune:
        args.run_metrics_exporter = True

    profile_cfg = dict(PROFILE_PRESETS[args.profile])

    def resolve_float(cli_value: Optional[float], env_key: str, fallback: float) -> float:
        if cli_value is not None:
            return float(cli_value)
        env_value = env_float(env_key)
        if env_value is not None:
            return env_value
        return float(fallback)

    profile_cfg["p99_multiplier"] = resolve_float(
        args.p99_multiplier, "P99_MULTIPLIER", profile_cfg["p99_multiplier"]
    )
    profile_cfg["mad_multiplier"] = resolve_float(
        args.mad_multiplier, "MAD_MULTIPLIER", profile_cfg["mad_multiplier"]
    )
    profile_cfg["epsilon_ms"] = resolve_float(args.epsilon_ms, "EPSILON_MS", profile_cfg["epsilon_ms"])

    budget_cap = args.p99_budget_ms
    if budget_cap is None:
        budget_cap = env_float("P99_BUDGET_MS")

    runner_id = resolve_runner_id(args.runner_id)
    bench = parse_bench(Path(args.bench_file))
    metrics = parse_prom_metrics(Path(args.metrics_file))
    baseline = load_baseline(Path(args.baseline_file), runner_id)

    allowed_ms = compute_allowed(
        baseline,
        mad_multiplier=profile_cfg["mad_multiplier"],
        p99_multiplier=profile_cfg["p99_multiplier"],
        epsilon_ms=profile_cfg["epsilon_ms"],
        budget_cap_ms=budget_cap,
    )

    decision_lines = [
        f"runner: {runner_id}",
        f"profile: {args.profile} ({profile_cfg['description']})",
        f"measured_p99_ms:    {bench.p99_ms:.4f}",
        f"measured_p999_ms:   {bench.p999_ms:.4f}",
        f"measured_p9999_ms:  {bench.p9999_ms:.4f}",
        f"allowed_p99_ms:     {allowed_ms:.4f}",
        f"median_p99_ms: {baseline['median_p99_ms']}",
        f"mad_p99_ms: {baseline['mad_p99_ms']}",
        f"p99_multiplier: {profile_cfg['p99_multiplier']}",
        f"mad_multiplier: {profile_cfg['mad_multiplier']}",
        f"epsilon_ms: {profile_cfg['epsilon_ms']}",
    ]

    force_digest = os.getenv("REQUIRE_DIGEST")
    require_digest = profile_cfg["require_digest"] or args.require_digest or (force_digest == "1")
    digest_ok = True
    expected_digest = baseline.get("digest_fnv")
    if expected_digest:
        decision_lines.append(f"digest_ref: {expected_digest}")
    if require_digest and expected_digest:
        digest_ok = bench.digest == expected_digest
        decision_lines.append(f"digest_measured: {bench.digest}")
    else:
        decision_lines.append(f"digest_measured: {bench.digest} (informational)")

    status = "PASS"
    if bench.p99_ms > allowed_ms + 1e-9:
        status = "FAIL"
    if require_digest and expected_digest and not digest_ok:
        status = "FAIL"

    # p99.9 gate: must be ≤ 3× allowed_ms (i.e. 3× the p99 budget)
    allowed_p999_ms = allowed_ms * 3.0
    if bench.p999_ms > 0.0 and bench.p999_ms > allowed_p999_ms + 1e-9:
        status = "FAIL"
        decision_lines.append(
            f"p999_gate: FAIL ({bench.p999_ms:.4f} ms > {allowed_p999_ms:.4f} ms)"
        )
    elif bench.p999_ms > 0.0:
        decision_lines.append(
            f"p999_gate: PASS ({bench.p999_ms:.4f} ms ≤ {allowed_p999_ms:.4f} ms)"
        )

    # p99.99 advisory: warn only (not a hard gate) — observable but not blocking
    if bench.p9999_ms > 0.0:
        decision_lines.append(
            f"p9999_advisory: {bench.p9999_ms:.4f} ms (informational)"
        )

    print("verdict:", status)
    for line in decision_lines:
        print(line)

    if args.run_metrics_exporter:
        export_metrics_bundle(
            Path(args.metrics_export_path),
            bench=bench,
            metrics=metrics,
            baseline=baseline,
            allowed_ms=allowed_ms,
            status=status,
            runner_id=runner_id,
            profile_name=args.profile,
            profile_cfg=profile_cfg,
            include_auto_tune=args.auto_tune,
        )

    return 0 if status == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
