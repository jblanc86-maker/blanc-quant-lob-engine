# SignalGrid Examples

A tiny CSV (`qa_demo.csv`) ships with three roles so you can exercise the CLI offline.

## Quick start

```sh
sg scan --from examples/qa_demo.csv --profile demo
sg sitrep --format table --dry-run
sg targets --min-score 80 --limit 5
```

The CSV columns line up with the canonical ingestion schema (role id, title, company, location, persona tag, headcount spike flag, mutual count, age). Extend the file or drop your own CSVs alongside it to test new scoring weights.

### Run the demo script

We've added a tiny Python demo runner that loads `qa_demo.csv` and prints a short summary. It's useful as a starter example for newcomers.

Usage:

```sh
python3 examples/run_demo.py
# or filter for headcount spikes only
python3 examples/run_demo.py --filter-spike
python3 examples/run_demo.py --min-mutuals 1
```

The script uses only the Python standard library and requires a usable Python 3 interpreter. If you use a virtual environment, activate it first.
