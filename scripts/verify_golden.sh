#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail
cd "$(git rev-parse --show-toplevel)"
actual="$(shasum -a 256 data/golden/itch_1m.bin | awk '{print $1}')"
want="$(cat data/golden/itch_1m.digest)"
mkdir -p artifacts
if [[ "$actual" != "$want" ]]; then
  echo "DIGEST DRIFT ❌ actual=$actual want=$want" | tee artifacts/verify.txt
  exit 2
fi
echo "Determinism PASS ✔︎ ($actual)" | tee artifacts/verify.txt
