#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Pin GitHub Action uses: lines in .github/workflows to commit SHAs
set -euo pipefail

# CLI: --dry-run (prints JSON proposals), --output-json <file> for dry-run; --commit (apply, default is to apply), --no-backup
DRY_RUN=0
OUTPUT_JSON=""
COMMIT=1
NO_BACKUP=0

while (( "$#" )); do
  case "$1" in
    --dry-run)
      DRY_RUN=1; shift;;
    --output-json)
      OUTPUT_JSON="$2"; shift 2;;
    --commit)
      COMMIT=1; shift;;
    --no-backup)
      NO_BACKUP=1; shift;;
    --help)
      echo "Usage: $0 [--dry-run] [--output-json file] [--no-backup]"; exit 0;;
    *)
      echo "Unknown option: $1"; exit 2;;
  esac
done

BASEDIR="$(cd "$(dirname "$0")/.." && pwd)"
WORKFLOWS_DIR="$BASEDIR/.github/workflows"
BACKUP_DIR="$WORKFLOWS_DIR/.backup_$(date -u +%Y%m%d%H%M%S)"

if [[ $NO_BACKUP -ne 1 && $DRY_RUN -eq 0 ]]; then
  mkdir -p "$BACKUP_DIR"
  cp -r "$WORKFLOWS_DIR"/* "$BACKUP_DIR" || true
  printf "Backed up workflows to %s\n" "$BACKUP_DIR"
fi

changed=0
proposals=()

for file in "$WORKFLOWS_DIR"/*.{yml,yaml}; do
  [[ -f "$file" ]] || continue
  tmpfile="${file}.tmp"
  cp "$file" "$tmpfile"
  updated=0
  while IFS= read -r line; do
    if [[ $line =~ uses:[[:space:]]*([^@[:space:]]+)@([^[:space:]#]+) ]]; then
      full_repo=${BASH_REMATCH[1]}
      # repo is always owner/repo; sometimes the action uses an extra path (e.g. 'owner/repo/path')
      # Extract owner/repo only
      owner_repo=$(echo "$full_repo" | awk -F'/' '{print $1"/"$2}')
      repo=$owner_repo
      ref=${BASH_REMATCH[2]}
      if [[ $ref =~ ^[0-9a-f]{40}$ ]]; then
        continue
      fi
  repo_url="https://github.com/${repo}.git"
      echo "Resolving $repo@$ref"
      sha=""
      if sha_line=$(git ls-remote --exit-code "$repo_url" "refs/tags/$ref^{}" 2>/dev/null || true); then
        sha=$(awk '{print $1; exit}' <<<"$sha_line")
      fi
      if [[ -z "$sha" ]] && sha_line=$(git ls-remote --exit-code "$repo_url" "refs/tags/$ref" 2>/dev/null || true); then
        sha=$(awk '{print $1; exit}' <<<"$sha_line")
      fi
      if [[ -z "$sha" ]] && sha_line=$(git ls-remote --exit-code "$repo_url" "refs/heads/$ref" 2>/dev/null || true); then
        sha=$(awk '{print $1; exit}' <<<"$sha_line")
      fi
      if [[ -z "$sha" ]] && [[ $ref =~ ^[0-9a-f]{7,40}$ ]]; then
        sha_line=$(git ls-remote --exit-code "$repo_url" "*${ref}*" 2>/dev/null || true) || sha_line=''
        sha=$(awk '{print $1; exit}' <<<"$sha_line")
      fi
      if [[ -z "$sha" ]]; then
        echo "Could not resolve $repo@$ref to a commit SHA; leaving as-is"
        continue
      fi
      old="${repo}@${ref}"
      new="${repo}@${sha}"
      if grep -qF "$old" "$tmpfile"; then
        if [[ $DRY_RUN -eq 1 ]]; then
          proposals+=("{\"file\":\"$file\",\"old\":\"$old\",\"new\":\"$new\"}")
        else
          echo "Pinning $old -> $new in $file"
          content=$(<"$tmpfile")
          content=${content//$old/$new}
          printf "%s" "$content" > "$tmpfile"
          updated=1
        fi
      fi
    fi
  done < "$file"
  if [[ $updated -eq 1 ]]; then
    mv "$tmpfile" "$file"
    changed=1
  else
    rm -f "$tmpfile"
  fi
done

if [[ $DRY_RUN -eq 1 ]]; then
  # Print proposals as a JSON array
  if [[ ${#proposals[@]} -ne 0 ]]; then
    joined=$(printf "%s\n" "${proposals[@]}" | paste -sd "," -)
    if [[ -n "$OUTPUT_JSON" ]]; then
      printf "%s\n" "[$joined]" > "$OUTPUT_JSON"
    fi
    printf "%s\n" "[$joined]"
  else
    echo "[]"
  fi
  if [[ ${#proposals[@]} -ne 0 ]]; then
    exit 0
  else
    exit 0
  fi
else
  if [[ $changed -eq 1 ]]; then
    echo "One or more files were updated; please review, git add, and commit."
  else
    echo "No updates required; all actions appear pinned to commit SHAs."
  fi
fi

exit 0
