#!/usr/bin/env python3
"""Tiny demo runner for examples/qa_demo.csv

Usage:
  python3 scripts/run_demo.py [path/to/qa_demo.csv]

This script loads the sample CSV and scores rows with a tiny heuristic so
you can see how to wire sample data into a minimal workflow.
"""
import csv
import sys
from pathlib import Path


def score_candidate(row: dict) -> int:
    """Compute a simple heuristic score for a candidate row.

    Heuristic (demo only):
    - headcount_spike == true => +20
    - mutuals * 5
    - recency bonus: max(0, 30 - age_days)
    """
    s = 0
    if row.get("headcount_spike", "").strip().lower() == "true":
        s += 20
    try:
        mutuals = int(row.get("mutuals", "0"))
    except Exception:
        mutuals = 0
    s += mutuals * 5
    try:
        age = int(row.get("age_days", "0"))
    except Exception:
        age = 0
    s += max(0, 30 - age)
    return s


def run_demo(csv_path: Path) -> None:
    if not csv_path.exists():
        print(f"File not found: {csv_path}")
        return
    with csv_path.open(newline="") as fh:
        reader = csv.DictReader(fh)
        rows = list(reader)

    scored = [(row, score_candidate(row)) for row in rows]
    scored.sort(key=lambda t: t[1], reverse=True)

    print("\nDemo workflow: scored candidates (highest first)\n")
    for row, s in scored:
        rid = row.get("role_id", "<no-id>")
        title = row.get("title", "<no-title>")
        company = row.get("company", "<no-company>")
        print(f"{rid}: {title} @ {company} — score={s}")


def main(argv: list[str] | None = None) -> int:
    argv = argv if argv is not None else sys.argv[1:]
    path = Path(argv[0]) if len(argv) >= 1 else Path("examples/qa_demo.csv")
    run_demo(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
