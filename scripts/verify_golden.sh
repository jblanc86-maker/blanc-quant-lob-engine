#!/usr/bin/env bash
set -euo pipefail
here="$(cd "$(dirname "$0")/.." && pwd)"
cd "$here"
actual="$(shasum -a 256 data/golden/itch_1m.bin | awk '{print $1}')"
want="$(cat data/golden/itch_1m.digest)"
if [[ "$actual" != "$want" ]]; then
  echo "DIGEST DRIFT ❌ actual=$actual want=$want"
  exit 2
fi
echo "Determinism PASS ✔︎ ($actual)"
