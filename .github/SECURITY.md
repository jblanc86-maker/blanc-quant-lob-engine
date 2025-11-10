# Security Policy

Thank you for helping keep this project and its users safe.

## Reporting a Vulnerability

- Preferred: Use GitHub Private Vulnerability Reporting (Security tab → “Report a vulnerability”). This sends a private report to the maintainers.
- If private reporting is unavailable, open a draft GitHub Security Advisory (Security tab → Advisories → “New draft advisory”).
- As a last resort, open an issue starting with "[SECURITY]" and minimal details requesting a secure contact channel. Do not include exploit details in public issues.

Please include:

- A clear description of the issue and impact
- Steps to reproduce or a proof-of-concept
- Affected commit/branch and environment details

## Supported Versions

Security fixes are provided for the latest `main` branch and the most recent tagged release. Older versions may not receive fixes.

## Response Targets (Goals)

- Acknowledge receipt within 3 business days
- Initial assessment within 7 business days
- Coordinated remediation plan after triage

These are goals, not guarantees, but we aim for timely and transparent communication.

## Disclosure and Credit

We follow coordinated disclosure. Downstream users are notified as needed. Reporters are credited in release notes unless they request anonymity.

## Scope

- In scope: Code and configuration in this repository
- Out of scope: Third‑party dependencies, forks you maintain, or deployments you operate. (Report dependency issues upstream.)

## Safe Harbor

Good‑faith security research is welcomed provided it:

- Respects privacy and data integrity
- Avoids service disruption or data exfiltration
- Uses only the minimum access required to demonstrate impact
- Allows a reasonable remediation window before public disclosure

If unsure, open a minimal clarification issue.
# Security Policy

## Supported Versions
We support the latest main branch and the most recent tagged release.

## Reporting a Vulnerability
Please email **security@yourdomain.com** with:
- A minimal, reproducible description (what, where, how to trigger)
- Commit hash / tag, OS + compiler, and any scripts or inputs used
- Impact assessment if known (e.g., crash, data corruption, RCE)

**Do not** open public issues for vulnerabilities. We will acknowledge within **2 business days**
and provide a remediation timeline after triage.

## Coordinated Disclosure
We appreciate coordinated disclosure. If you need a CVE, let us know; we can coordinate.

## Keys & Artifacts
Official releases are signed (tag notes will include details). Only trust tags from this repo’s verified maintainers.
