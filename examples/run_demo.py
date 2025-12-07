#!/usr/bin/env python3
"""Simple demo runner for examples/qa_demo.csv

Usage:
  python3 examples/run_demo.py [--file examples/qa_demo.csv] [--filter-spike]

Prints a short summary and optionally filters rows with headcount_spike==true.
"""
import csv
import argparse
from pathlib import Path


def load_csv(path: Path):
    rows = []
    with path.open(newline="") as fh:
        reader = csv.DictReader(fh)
        for r in reader:
            # normalize fields
            r['headcount_spike'] = r.get('headcount_spike','').lower() in ('1','true','yes','y')
            # numeric conversions
            try:
                r['mutuals'] = int(r.get('mutuals','0'))
            except Exception:
                r['mutuals'] = 0
            try:
                r['age_days'] = int(r.get('age_days','0'))
            except Exception:
                r['age_days'] = 0
            rows.append(r)
    return rows


def summary(rows):
    total = len(rows)
    spikes = [r for r in rows if r['headcount_spike']]
    avg_mutuals = sum(r['mutuals'] for r in rows) / total if total else 0
    avg_age = sum(r['age_days'] for r in rows) / total if total else 0
    return {
        'total': total,
        'spike_count': len(spikes),
        'avg_mutuals': avg_mutuals,
        'avg_age_days': avg_age,
        'top_spikes': spikes[:5],
    }


def print_report(path, rows, report):
    print(f"Loaded: {path} -> {report['total']} rows")
    print(f"Headcount spikes: {report['spike_count']}\n")
    print(f"Average mutuals: {report['avg_mutuals']:.2f}")
    print(f"Average age (days): {report['avg_age_days']:.1f}\n")
    if report['top_spikes']:
        print("Top headcount-spike roles:")
        for r in report['top_spikes']:
            print(f" - {r['role_id']}: {r['title']} @ {r['company']} (mutuals={r['mutuals']}, age={r['age_days']}d)")


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--file', '-f', default='examples/qa_demo.csv')
    p.add_argument('--filter-spike', action='store_true', help='Only print spike rows')
    p.add_argument('--min-mutuals', type=int, default=0, help='Filter by minimum mutuals')
    args = p.parse_args()

    path = Path(args.file)
    if not path.exists():
        print(f"Error: file not found: {path}")
        raise SystemExit(2)

    rows = load_csv(path)
    rep = summary(rows)

    print_report(path, rows, rep)

    if args.filter_spike or args.min_mutuals > 0:
        filtered = [r for r in rows if (not args.filter_spike or r['headcount_spike']) and r['mutuals'] >= args.min_mutuals]
        print('\nFiltered results:')
        for r in filtered:
            print(f"{r['role_id']},{r['title']},{r['company']},{r['mutuals']},{r['age_days']},spike={r['headcount_spike']}")


if __name__ == '__main__':
    main()
