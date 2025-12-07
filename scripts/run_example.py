#!/usr/bin/env python3
"""Small example: read examples/qa_demo.csv and print rows + a short summary.

Usage:
  python3 scripts/run_example.py

This is intended as a tiny starter/demo script for the repo's `examples/` data.
"""
import csv
import json
from pathlib import Path

SRC = Path(__file__).resolve().parents[1] / "examples" / "qa_demo.csv"


def load_csv(path: Path):
    rows = []
    with path.open("r", encoding="utf-8") as fh:
        # tolerate fenced code blocks in the sample file (strip leading/trailing ``` lines)
        first = fh.readline()
        if first.strip().startswith("```"):
            # read remainder
            text = fh.read()
            # remove possible trailing fence
            text = text.rsplit('```', 1)[0]
            # parse from text
            reader = csv.DictReader(text.strip().splitlines())
            for r in reader:
                rows.append(r)
            return rows
        else:
            # fallback: rewind and use normal reader
            fh.seek(0)
            reader = csv.DictReader(fh)
            for r in reader:
                rows.append(r)
            return rows


def summary(rows):
    out = {
        "count": len(rows),
        "roles": [r.get("role_id") for r in rows],
    }
    return out


def main():
    if not SRC.exists():
        print(f"Example data not found: {SRC}")
        return

    rows = load_csv(SRC)
    print("Loaded rows:")
    for r in rows:
        print(f"- {r.get('role_id')}: {r.get('title')} @ {r.get('company')} ({r.get('location')})")

    print()
    s = summary(rows)
    print("Summary JSON:")
    print(json.dumps(s, indent=2))


if __name__ == "__main__":
    main()
