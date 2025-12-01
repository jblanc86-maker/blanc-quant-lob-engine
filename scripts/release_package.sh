#!/usr/bin/env bash
set -euo pipefail
# Lightweight release packaging script
# Produces: manifest.json, rights_manifest.json, <out>.zip and optional GPG signature

usage(){
  cat <<EOF
Usage: $0 [--build-dir BUILD] [--art-dir ART] [--out-dir OUT] [--git-sha SHA] [--sign]

Defaults:
  BUILD=build
  ART=artifacts
  OUT=artifacts/release

Outputs placed under OUT. Manifest contains file list with sha256 and size.
If --sign is passed and gpg is available, a detached signature OUT/archive.zip.sig
will be created.
EOF
}

BUILD_DIR=build
ART_DIR=artifacts
OUT_DIR=${ART_DIR}/release
GIT_SHA=""
DO_SIGN=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --build-dir) BUILD_DIR="$2"; shift 2 ;;
    --art-dir) ART_DIR="$2"; OUT_DIR=${ART_DIR}/release; shift 2 ;;
    --out-dir) OUT_DIR="$2"; shift 2 ;;
    --git-sha) GIT_SHA="$2"; shift 2 ;;
    --sign) DO_SIGN=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown arg: $1"; usage; exit 2 ;;
  esac
done

mkdir -p "$OUT_DIR"

archive_name="blanc-lob-engine-${GIT_SHA:-snapshot}.zip"
archive_path="$OUT_DIR/$archive_name"

tmpdir=$(mktemp -d)
cleanup(){ rm -rf "$tmpdir"; }
trap cleanup EXIT

# Files to include: binary if present, artifacts dir contents, README and LICENSE
if [ -x "${BUILD_DIR}/bin/replay" ]; then
  mkdir -p "$tmpdir/bin"
  cp "${BUILD_DIR}/bin/replay" "$tmpdir/bin/"
fi

if [ -d "${ART_DIR}" ]; then
  mkdir -p "$tmpdir/artifacts"
  # copy only non-empty files; preserve structure
  (cd "${ART_DIR}" && tar -czf - .) | tar -xzf - -C "$tmpdir/artifacts"
fi

cp -n README.md "$tmpdir/" || true
cp -n LICENSE "$tmpdir/" || true

# Build manifest.json
manifest="$OUT_DIR/manifest.json"
rm -f "$manifest"
echo '{"files":[' > "$manifest"
first=1
find "$tmpdir" -type f | sort | while read -r f; do
  rel=${f#$tmpdir/}
  sha=$(shasum -a 256 "$f" | awk '{print $1}')
  size=$(stat -f%z "$f" 2>/dev/null || stat -c%s "$f" 2>/dev/null || echo 0)
  if [ "$first" -eq 1 ]; then first=0; else echo ',' >> "$manifest"; fi
  printf '\n  {"path":"%s","sha256":"%s","size":%s}' "$rel" "$sha" "$size" >> "$manifest"
done
echo -e '\n]}' >> "$manifest"

# rights_manifest.json (minimal template)
rights_manifest="$OUT_DIR/rights_manifest.json"
cat > "$rights_manifest" <<EOF
{
  "git_sha": "${GIT_SHA}",
  "build_dir": "${BUILD_DIR}",
  "rights": "SBIR-RESTRICTED-TECHNOLOGY",
  "generated_by": "release_package.sh",
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
EOF

# Create zip archive
pushd "$tmpdir" >/dev/null
zip -r "$archive_path" . >/dev/null
popd >/dev/null

echo "Created archive: $archive_path"
echo "Created manifest: $manifest"
echo "Created rights manifest: $rights_manifest"

if [ "$DO_SIGN" -eq 1 ]; then
  if command -v gpg >/dev/null 2>&1; then
    sig_path="$archive_path.sig"
    gpg --batch --yes --detach-sign -o "$sig_path" "$archive_path"
    echo "Created signature: $sig_path"
  else
    echo "gpg not found; skipping signature" >&2
  fi
fi

echo "Release packaging complete. Output directory: $OUT_DIR"
