#!/usr/bin/env python3
"""Generate a Shields.io-compatible JSON badge summarizing CodeQL alerts."""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import sys
import urllib.error
import urllib.request
from typing import Dict, List, Optional

API_TEMPLATE = "https://api.github.com/repos/{owner}/{repo}/code-scanning/alerts?tool_name=CodeQL&state={state}&per_page=100"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner", default=os.environ.get("REPO_OWNER", "jblanc86-maker"), help="GitHub repository owner")
    parser.add_argument("--repo", default=os.environ.get("REPO_NAME", "blanc-quant-lob-engine"), help="GitHub repository name")
    parser.add_argument("--state", default="open", choices=["open", "closed", "dismissed"], help="Alert state to summarize")
    parser.add_argument("--label", default="CodeQL Alerts", help="Badge label text")
    parser.add_argument("--output", default="badges/codeql_alerts.json", help="Destination JSON file")
    parser.add_argument("--token", default=os.environ.get("GITHUB_TOKEN"), help="GitHub token (falls back to env var)")
    return parser.parse_args()


def _parse_next_link(link_header: Optional[str]) -> Optional[str]:
    if not link_header:
        return None
    parts = [p.strip() for p in link_header.split(",")]
    for part in parts:
        if part.endswith('rel="next"'):
            start = part.find("<") + 1
            end = part.find(">", start)
            if start > 0 and end > start:
                return part[start:end]
    return None


def fetch_alerts(owner: str, repo: str, state: str, token: Optional[str]) -> List[Dict]:
    alerts: List[Dict] = []
    url = API_TEMPLATE.format(owner=owner, repo=repo, state=state)
    headers = {"Accept": "application/vnd.github+json"}
    if token:
        headers["Authorization"] = f"Bearer {token}"

    while url:
        request = urllib.request.Request(url, headers=headers)
        try:
            with urllib.request.urlopen(request) as response:
                payload = json.loads(response.read().decode("utf-8"))
                alerts.extend(payload)
                url = _parse_next_link(response.headers.get("Link"))
        except urllib.error.HTTPError as exc:  # pragma: no cover
            sys.stderr.write(f"Failed to fetch alerts ({exc.code}): {exc.reason}\n")
            return []
    return alerts


def summarize(alerts: List[Dict]) -> Dict[str, int]:
    summary = {"critical": 0, "high": 0, "medium": 0, "low": 0, "warning": 0, "error": 0}
    for alert in alerts:
        severity = (alert.get("rule") or {}).get("severity") or alert.get("severity") or "unknown"
        severity = severity.lower()
        if severity in summary:
            summary[severity] += 1
        else:
            summary.setdefault("other", 0)
            summary["other"] += 1
    summary["total"] = len(alerts)
    return summary


def pick_color(total: int, critical: int) -> str:
    if total == 0:
        return "success"
    if critical > 0:
        return "critical"
    if total <= 5:
        return "warning"
    return "orange"


def main() -> int:
    args = parse_args()
    alerts = fetch_alerts(args.owner, args.repo, args.state, args.token)
    summary = summarize(alerts)
    total = summary.get("total", 0)
    critical = summary.get("critical", 0)
    message = f"{total} open"
    if critical:
        message += f" · {critical} critical"

    badge = {
        "schemaVersion": 1,
        "label": args.label,
        "message": message,
        "color": pick_color(total, critical),
    }

    output_path = pathlib.Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(badge, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {output_path} -> {badge['message']}")
    return 0


if __name__ == "__main__":  # pragma: no cover
    raise SystemExit(main())
