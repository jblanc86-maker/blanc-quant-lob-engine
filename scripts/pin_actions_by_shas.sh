#!/usr/bin/env bash
set -euo pipefail
shopt -s nullglob
WF_DIR=".github/workflows"
FILES=("$WF_DIR"/*.{yml,yaml})
[ ${#FILES[@]} -eq 0 ] && { echo "No workflow files"; exit 0; }

backup_dir="$WF_DIR/.backup_$(date +%Y%m%d_%H%M%S)"; mkdir -p "$backup_dir"
is_sha(){ [[ "$1" =~ ^[0-9a-fA-F]{40}$ ]]; }

resolve_sha(){
  local repo="$1" ref="$2" resp type sha
  if ! resp=$(gh api "repos/${repo}/git/refs/tags/${ref}" -q .object 2>/dev/null); then
    resp=$(gh api "repos/${repo}/git/refs/heads/${ref}" -q .object 2>/dev/null) || return 1
  fi
  type=$(jq -r '.type' <<<"$resp"); sha=$(jq -r '.sha' <<<"$resp")
  if [[ "$type" == "tag" ]]; then
    sha=$(gh api "repos/${repo}/git/tags/${sha}" -q .object.sha)
  fi
  echo "$sha"
}

for f in "${FILES[@]}"; do
  cp "$f" "$backup_dir/$(basename "$f")"
  tmp="${f}.tmp"; : > "$tmp"
  while IFS= read -r line; do
    if [[ "$line" =~ uses:\ ([^@#[:space:]]+/[^@#[:space:]]+)@([^[:space:]#]+) ]]; then
      repo="${BASH_REMATCH[1]}"; ref="${BASH_REMATCH[2]}"
      if is_sha "$ref"; then echo "$line" >> "$tmp"; continue; fi
      if sha="$(resolve_sha "$repo" "$ref")" && is_sha "$sha"; then
        indent="${line%%uses:*}"
        echo "${indent}# ${repo}@${ref}" >> "$tmp"
        echo "${indent}uses: ${repo}@${sha}" >> "$tmp"
        echo "Pinned: ${repo}@${ref} → ${sha}"
      else
        echo "WARN: could not resolve ${repo}@${ref}; leaving as-is" >&2
        echo "$line" >> "$tmp"
      fi
    else
      echo "$line" >> "$tmp"
    fi
  done < "$f"
  mv "$tmp" "$f"
done
echo "Backups saved to: $backup_dir"
