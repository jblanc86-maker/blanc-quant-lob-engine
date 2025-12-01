# Deliverable Marking Checklist (CI-ready)

This checklist is intended to be used as part of a CI job that prepares a deliverable package for government customers and partners. The goal is to ensure artifacts are properly marked, documented, and packaged to preserve data/IP rights and traceability.

Checklist

1. Source of truth
   - Confirm the Git SHA and branch for the build.
   - Record the build timestamp and environment (OS, compiler, build flags).

2. Build artifacts
   - Binaries (.tar.gz) — include metadata JSON (hashes, signed-by, build environment).
   - Test artifacts (bench.jsonl, metrics.prom) — include last run digest and summary.
   - Release notes and changelog.

3. IP & data rights markings
   - Prepend a rights header to any PDF/Report or deliverable (SBIR legend or Company LTA) in the artifact pack.
   - If using SBIR-related funding: attach the SBIR legend (e.g., "SBIR Data Rights Notice — Do not release outside of the Government") where applicable.
   - For any code or data that is delivered as source, mark the files with rights (e.g., `LIMITED RIGHTS`), and include a `rights_manifest.json` for each deliverable.

4. Packaging
   - Create a signed ZIP with: binaries, metadata JSON, bench summary, rights_manifest, README, and a brief 'instructions.txt'.
   - Include a `manifest.json` enumerating included files with SHA256 and size.
   - Optionally include a PGP signature of the artifact and metadata.

5. CI gates
   - Validate that the rights manifest (README + rights_manifest.json) are included.
   - Confirm tests passed and bench-summary exists for the run.
   - Confirm no `UNMARKED` or accidental artifacts are present; if so, fail the job and send an alert.

6. Deliverable labels
   - Set `dist: public` or `dist: SBIR` or `dist: limited` fields in metadata.
   - Add `build_id`, `git_sha`, `build_time`, `signed_by` in metadata JSON.

7. Human review
   - Notify the Tech Transfer / Contracts contact for any non-trivial delivery (especially SBIR/OTAs).
   - Optionally attach a short STEM brief and diagrams.

Automation suggestions

- Add a CI job to produce deliverable artifacts and attach rights/legends based on the branch (e.g., `release/*` → `dist: public` with export checks; `sbir/*` → `dist: sbir` with SBIR legend).
- Use a templated `rights_manifest.json` for the most common funding cases, and allow overrides via an environment override.
- Ensure `artifact.zip` is signed and the signature published in a side-channel (e.g., signed artifacts on S3 and the signature in GitHub release notes).

Template `rights_manifest.json` example

```json
{
  "artifact": "build/software-v1.2.3.tar.gz",
  "dist": "SBIR",
  "rights": "SBIR_DATA_RIGHTS",
  "build_id": "${GITHUB_RUN_ID}",
  "git_sha": "${GIT_SHA}",
  "signed_by": "${SIGNER}"
}
```

Include these checks in your `release` job to ensure deliverables are consistent and marked.
