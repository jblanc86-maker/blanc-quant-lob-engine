#!/usr/bin/env python3
"""Update or create a GitHub Release using GitHub's versioned REST API.

GitHub's REST API is versioned; callers should send:
  X-GitHub-Api-Version: 2026-03-10

This script is intended for GitHub Actions, using the implicit `GITHUB_TOKEN`.

What it does
------------
- GET /repos/{owner}/{repo}/releases/tags/{tag}
  - If it exists: PATCH /repos/{owner}/{repo}/releases/{release_id}
  - If not: POST /repos/{owner}/{repo}/releases

It updates metadata (name/body/draft/prerelease) and does NOT upload assets.
"""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import sys
import urllib.error
import urllib.request

API_VERSION = "2026-03-10"
DEFAULT_API = "https://api.github.com"


def _request(method: str, url: str, token: str | None, payload: dict | None = None) -> tuple[int, object | None]:
    headers = {
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": API_VERSION,
        "User-Agent": "blanc-quant-lob-engine-release-script",
    }
    if token:
        headers["Authorization"] = f"Bearer {token}"

    data = None
    if payload is not None:
        data = json.dumps(payload).encode("utf-8")
        headers["Content-Type"] = "application/json"

    req = urllib.request.Request(url, data=data, headers=headers, method=method)

    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            status = resp.status
            raw = resp.read()
    except urllib.error.HTTPError as e:
        status = e.code
        raw = e.read() if e.fp else b""

    if not raw:
        return status, None

    try:
        return status, json.loads(raw.decode("utf-8"))
    except Exception:
        return status, {"raw": raw.decode("utf-8", errors="replace")}


def get_release_by_tag(api: str, owner: str, repo: str, tag: str, token: str | None) -> dict | None:
    status, data = _request("GET", f"{api}/repos/{owner}/{repo}/releases/tags/{tag}", token)
    if status == 200 and isinstance(data, dict):
        return data
    if status == 404:
        return None
    raise RuntimeError(f"GET release by tag failed (status {status}): {data}")


def create_release(
    api: str,
    owner: str,
    repo: str,
    token: str,
    tag: str,
    target_commitish: str,
    name: str,
    body: str,
    draft: bool,
    prerelease: bool,
    make_latest: str | None,
) -> dict:
    payload: dict = {
        "tag_name": tag,
        "target_commitish": target_commitish,
        "name": name,
        "body": body,
        "draft": draft,
        "prerelease": prerelease,
        "generate_release_notes": False,
    }
    if make_latest is not None:
        payload["make_latest"] = make_latest

    status, data = _request("POST", f"{api}/repos/{owner}/{repo}/releases", token, payload)
    if status != 201 or not isinstance(data, dict):
        raise RuntimeError(f"Create release failed (status {status}): {data}")
    return data


def update_release(
    api: str,
    owner: str,
    repo: str,
    token: str,
    release_id: int,
    name: str,
    body: str,
    draft: bool,
    prerelease: bool,
    make_latest: str | None,
) -> dict:
    payload: dict = {
        "name": name,
        "body": body,
        "draft": draft,
        "prerelease": prerelease,
    }
    if make_latest is not None:
        payload["make_latest"] = make_latest

    status, data = _request("PATCH", f"{api}/repos/{owner}/{repo}/releases/{release_id}", token, payload)
    if status != 200 or not isinstance(data, dict):
        raise RuntimeError(f"Update release failed (status {status}): {data}")
    return data


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--owner", required=True)
    p.add_argument("--repo", required=True)
    p.add_argument("--tag", required=True)
    p.add_argument("--name", required=True)
    p.add_argument("--target-commitish", required=True)
    p.add_argument("--body-file", required=True)
    p.add_argument("--api", default=DEFAULT_API)
    p.add_argument("--draft", action="store_true")
    p.add_argument("--prerelease", action="store_true")
    p.add_argument("--make-latest", choices=["true", "false", "legacy"], default=None)
    args = p.parse_args(argv)

    token = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN")
    if not token:
        print("Missing GITHUB_TOKEN (or GH_TOKEN) in environment", file=sys.stderr)
        return 2

    body_path = pathlib.Path(args.body_file)
    if not body_path.exists():
        print(f"Body file not found: {body_path}", file=sys.stderr)
        return 2

    body = body_path.read_text(encoding="utf-8")

    existing = get_release_by_tag(args.api, args.owner, args.repo, args.tag, token)
    if existing is None:
        created = create_release(
            args.api,
            args.owner,
            args.repo,
            token,
            args.tag,
            args.target_commitish,
            args.name,
            body,
            args.draft,
            args.prerelease,
            args.make_latest,
        )
        print(json.dumps({"action": "created", "id": created.get("id"), "html_url": created.get("html_url")}, indent=2))
        return 0

    release_id = existing.get("id")
    if not isinstance(release_id, int):
        raise RuntimeError(f"Existing release has no numeric id: {existing}")

    updated = update_release(
        args.api,
        args.owner,
        args.repo,
        token,
        release_id,
        args.name,
        body,
        args.draft,
        args.prerelease,
        args.make_latest,
    )
    print(json.dumps({"action": "updated", "id": updated.get("id"), "html_url": updated.get("html_url")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
