#!/usr/bin/env python3
"""Prepare the approved Krimble toolbox artwork without dropping any tools.

Usage:
  python3 packaging/android/prepare-toolbox-icons.py krimble_toolbox_icons_1.0.45-beta.zip

Reads the 34 transparent icons, checks coverage, resolves icon names from
installed tool factories, and emits a staging directory plus a report.
Does not change the tool registry or remove existing icon resources.
"""
import argparse
import json
import re
import shutil
import sys
import tempfile
import zipfile
from pathlib import Path

# Entries are source artwork filename stems -> actual registered icon names,
# only where the original tool factory has been explicitly verified.
VERIFIED = {
    "01_02_rectangular_select": "tool_rect_selection",
    "01_03_elliptical_select": "tool_elliptical_selection",
    "01_04_freehand_select": "tool_outline_selection",
    "01_05_polygonal_select": "tool_polygonal_selection",
    "03_02_freehand_brush": "krita_tool_freehand",
}

def registry_icons(root):
    found = set()
    for file in (root / "plugins" / "tools").rglob("*.h"):
        try:
            source = file.read_text(encoding="utf-8")
        except (OSError, UnicodeError):
            continue
        found.update(re.findall(
            r'setIconName\s*\(\s*koIconNameCStr\s*\(\s*"([^"]+)"', source))
    return found

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path)
    parser.add_argument("--output", type=Path, default=Path("build/krimble-toolbox-icon-staging"))
    parser.add_argument("--source-root", type=Path, default=Path("."))
    args = parser.parse_args()
    root = args.source_root.resolve()
    available = registry_icons(root)
    if not available:
        sys.exit("Cannot inventory tool factories; run from the Krimble repository root.")
    args.output.mkdir(parents=True, exist_ok=True)
    applied = []
    pending = []
    with zipfile.ZipFile(args.archive) as z:
        pngs = sorted(name for name in z.namelist()
                      if name.lower().endswith(".png") and not name.startswith("__MACOSX/"))
        if len(pngs) != 34:
            sys.exit(f"Expected 34 illustrated icons, found {len(pngs)}; no output installed")
        with tempfile.TemporaryDirectory() as tmp:
            for member in pngs:
                stem = Path(member).stem
                target = VERIFIED.get(stem)
                if target and target in available:
                    dest = args.output / (target + ".png")
                    dest.write_bytes(z.read(member))
                    applied.append({"artwork": stem, "icon_name": target})
                else:
                    pending.append(stem)
    report = {
        "registered_icons_found": len(available),
        "staged_verified_replacements": applied,
        "awaiting_exact_tool_mapping": pending,
        "unmodified_original_icons": sorted(available - set(VERIFIED.values())),
        "note": "Staged only; package integration requires complete mapping and QRC/build setup."
    }
    (args.output / "coverage.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Staged {len(applied)} known icons; {len(pending)} await mapping.")
    print(f"Coverage report: {args.output / 'coverage.json'}")
    if pending:
        print("No remaining tools or icon resources were removed.")

if __name__ == "__main__":
    main()
