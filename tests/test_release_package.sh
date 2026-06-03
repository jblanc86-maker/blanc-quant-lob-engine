#!/usr/bin/env bash
set -euo pipefail
# Small unit test for scripts/release_package.sh

TMPROOT=$(mktemp -d)
cleanup(){ rm -rf "$TMPROOT"; }
trap cleanup EXIT
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
REPO_ROOT=$(cd -- "${SCRIPT_DIR}/.." && pwd)

BUILD_DIR=$TMPROOT/build
ART_DIR=$TMPROOT/artifacts
OUT_DIR=$ART_DIR/release
mkdir -p "$BUILD_DIR/bin" "$ART_DIR"

# Create a fake binary
cat > "$BUILD_DIR/bin/replay" <<'EOF'
#!/usr/bin/env bash
echo "fake replay"
EOF
chmod +x "$BUILD_DIR/bin/replay"

# Create sample artifact files
echo '{"sample":true}' > "$ART_DIR/bench.jsonl"
echo '# HELP dummy metric' > "$ART_DIR/metrics.prom"

GIT_SHA=testsha123
PROJECT_VERSION=$(python3 - "${REPO_ROOT}/CMakeLists.txt" <<'PY'
import pathlib
import re
import sys

text = pathlib.Path(sys.argv[1]).read_text(encoding="utf-8")
match = re.search(r"project\([^)]*VERSION\s+([0-9.]+)", text, re.IGNORECASE | re.DOTALL)
print(match.group(1) if match else "")
PY
)

# Run the packaging script
SCRIPT=./scripts/release_package.sh
if [ ! -x "$SCRIPT" ]; then
  echo "$SCRIPT missing or not executable" >&2
  exit 2
fi

echo "Running release_package.sh against synthetic build/artifacts..."
"$SCRIPT" --build-dir "$BUILD_DIR" --art-dir "$ART_DIR" --out-dir "$OUT_DIR" --git-sha "$GIT_SHA"

# Assert outputs exist
ZIPFILE="$OUT_DIR/blanc_lob_engine_${PROJECT_VERSION}.zip"
MANIFEST="$OUT_DIR/manifest.json"
RIGHTS="$OUT_DIR/rights_manifest.json"

if [ ! -f "$ZIPFILE" ]; then
  echo "Missing archive: $ZIPFILE" >&2
  exit 3
fi
if [ ! -f "$MANIFEST" ]; then
  echo "Missing manifest: $MANIFEST" >&2
  exit 4
fi
if [ ! -f "$RIGHTS" ]; then
  echo "Missing rights manifest: $RIGHTS" >&2
  exit 5
fi

echo "Verifying manifest content contains expected paths..."
grep -q 'bin/replay' "$MANIFEST" || (echo "manifest missing bin/replay" >&2; exit 6)
grep -q 'artifacts/bench.jsonl' "$MANIFEST" || (echo "manifest missing artifacts/bench.jsonl" >&2; exit 7)
grep -q "\"version\": \"${PROJECT_VERSION}\"" "$RIGHTS" || (echo "rights manifest missing version" >&2; exit 8)

echo "Packaging unit test passed. Outputs in: $OUT_DIR"
