# SignalGrid Safe Rename Plan

The following sequence keeps history intact while renaming Lucy Library modules to their SignalGrid counterparts. Run the steps from repo root inside a fresh branch (for example `git checkout -b chore/signalgrid-rename`).

## 1. Create the new package layout

```sh
mkdir -p signalgrid/core vortex ops
```

If the project already uses a top-level `signalgrid/` package, adjust paths accordingly.

## 2. Move and stage modules with history

```sh
git mv scoring.py vortex/engine.py
printf "# default VORTEX weights\n" > vortex/weights.yaml

git mv night_ops_cli.py sg-cli
chmod +x sg-cli

git mv tourctl.py ops/tourctl.py

git mv canonical.py signalgrid/core/canonical.py

git mv change_detection.py signalgrid/core/change_detection.py
```

Commit each logical move individually if you want bisect-friendly history (for example `git commit -m "refactor: move scoring.py into vortex"`).

## 3. Wire up the new CLI entry point

1. Update `pyproject.toml`/`setup.cfg` to expose `sg = "sg_cli:main"` (or the chosen module name) under `[project.scripts]`.
2. Ensure `sg-cli` imports from `signalgrid.core` and `vortex.engine` instead of legacy modules.
3. Add integration tests that call `sg sitrep --dry-run` to guard future changes.

## 4. Fix imports and references

Run a quick search-and-replace for the old module names:

```sh
rg -n "scoring" -g"*.py"
rg -n "night_ops_cli" -g"*.py"
rg -n "tourctl" -g"*.py"
```

For each match, update imports to the new package paths (`from vortex.engine import score`, etc.).

## 5. Update documentation and manifests

- Point README/module docs to the new paths (already covered in the README rewrite).
- If there are manifests or config files referencing the old script names, update them in place.
- Regenerate any API docs (`make docs` or `pydoc-markdown`) to verify the new module names appear.

## 6. Run the full suite

```sh
python -m pip install -e .[dev]
pytest -q
sg sitrep --dry-run
```

Only merge after CI (including the smoke SITREP workflow) passes on the rename branch.
