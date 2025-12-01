#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Minimal smoke test for CPU pin telemetry integration
set -euo pipefail

ART_DIR="/tmp/lob-cpu-pin-test.$$"
CPU_PIN=0
mkdir -p "$ART_DIR"

echo "Running bench to verify cpu_pin telemetry..."
ART_DIR="$ART_DIR" CPU_PIN=$CPU_PIN make bench

if ! grep -qE '^lob_cpu_pin\s+0$' "$ART_DIR/metrics.prom"; then
  echo "ERROR: lob_cpu_pin 0 not found in $ART_DIR/metrics.prom"
  exit 1
fi

if ! tail -n 1 "$ART_DIR/bench.jsonl" | grep -q '"summary":true'; then
  echo "ERROR: Summary line not found in $ART_DIR/bench.jsonl"
  exit 1
fi

echo "CPU pin telemetry smoke test PASSED: lob_cpu_pin 0 found and bench summary present"
