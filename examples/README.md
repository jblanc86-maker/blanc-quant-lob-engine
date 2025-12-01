# SignalGrid Examples

A tiny CSV (`qa_demo.csv`) ships with three roles so you can exercise the CLI offline.

## Quick start

```sh
sg scan --from examples/qa_demo.csv --profile demo
sg sitrep --format table --dry-run
sg targets --min-score 80 --limit 5
```

The CSV columns line up with the canonical ingestion schema (role id, title, company, location, persona tag, headcount spike flag, mutual count, age). Extend the file or drop your own CSVs alongside it to test new scoring weights.
